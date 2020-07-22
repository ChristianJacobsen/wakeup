#include <algorithm>
#include <iostream>
#include <optional>
#include <string>

#include <boost/algorithm/string/split.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include <boost/asio/ip/udp.hpp>
#include <fmt/printf.h>
#include <cxxopts.hpp>

namespace asio = boost::asio;

std::optional<std::vector<std::uint8_t>> mac_address_to_byte_vector(std::string_view mac_address);

std::vector<std::uint8_t> create_wake_on_lan_payload(const std::vector<std::uint8_t> &mac_address_byte_vector);

int send_payload(const std::vector<uint8_t> &payload, std::uint16_t port);

int main(int argc, char **argv) {
    cxxopts::Options options{"wakeup", "Send Wake On LAN Magic Packets to the target MAC-address."};

    options.add_options()
            ("mac", "", cxxopts::value<std::string>());
    options.parse_positional("mac");
    options.positional_help("<MAC-address>");

    options.add_options("Generic")
            ("h,help", "Display this message")
            ("v,version", "Print the version number");

    options.add_options("Program")
            ("p,port", "The port to send the Magic Packet to", cxxopts::value<std::uint16_t>()->default_value("0"));

    const auto optional_results = [&argc, &argv, &options]() -> std::optional<cxxopts::ParseResult> {
        try {
            return options.parse(argc, argv);
        }
        catch (const cxxopts::argument_incorrect_type &error) {
            fmt::print("Incorrect argument type error: {}\n", error.what());
            return std::nullopt;
        }
        catch (const std::exception &error) {
            fmt::print("Unknown error: {}\n", error.what());
            return std::nullopt;
        }
    }();

    if (!optional_results.has_value()) {
        return EXIT_FAILURE;
    }

    const auto &result = optional_results.value();

    if (result.count("help")) {
        fmt::print("{}\n", options.help());
        return EXIT_SUCCESS;
    }

    if (result.count("version")) {
        fmt::print("{}\n", PROGRAM_VERSION);
        return EXIT_SUCCESS;
    }

    if (!result.count("mac")) {
        fmt::print("Missing MAC-address\n");
        return EXIT_FAILURE;
    }

    const auto mac_address = result["mac"].as<std::string>();
    const auto port = result["port"].as<std::uint16_t>();

    const auto mac_address_byte_vector = mac_address_to_byte_vector(mac_address);
    if (!mac_address_byte_vector.has_value()) {
        return EXIT_FAILURE;
    }

    const auto payload = create_wake_on_lan_payload(mac_address_byte_vector.value());

    return send_payload(payload, port);
}

std::optional<std::vector<std::uint8_t>> mac_address_to_byte_vector(std::string_view mac_address) {
    // Split the MAC-address into 6 parts
    std::vector<std::string> string_parts{};
    string_parts.reserve(6);
    boost::split(string_parts, mac_address, [](const auto &c) { return c == ':'; });

    if (string_parts.size() != 6) {
        fmt::print("Invalid MAC-address format\n");
        return std::nullopt;
    }

    // Convert all string representations to byte representations
    std::vector<std::uint8_t> parts{};
    parts.reserve(6);
    try {
        std::transform(cbegin(string_parts), cend(string_parts), std::back_inserter(parts), [](const auto &part) {
            return static_cast<std::uint8_t>(std::stoi(part, 0, 16));
        });
    }
    catch (const std::invalid_argument &error) {
        fmt::print("Could not convert MAC-address string representation to byte representation: {}\n", error.what());
        return std::nullopt;
    }
    catch (const std::exception &error) {
        fmt::print("Unknown error: {}\n", error.what());
        return std::nullopt;
    }

    return parts;
}

std::vector<std::uint8_t> create_wake_on_lan_payload(const std::vector<std::uint8_t> &mac_address_byte_vector) {
    std::vector<std::uint8_t> payload{};
    payload.reserve(102);

    // Create the 6 x 0xff prefix
    for (std::size_t i{0}; i < 6; ++i) {
        payload.emplace_back(0xff);
    }

    // Append the MAC-address 16 times
    for (std::size_t i{0}; i < 16ul; ++i) {
        for (const auto &byte : mac_address_byte_vector) {
            payload.emplace_back(byte);
        }
    }

    return payload;
}

int send_payload(const std::vector<uint8_t> &payload, std::uint16_t port) {
    asio::io_context ctx{};
    asio::ip::udp::socket socket{ctx};

    try {
        socket.open(asio::ip::udp::v4());
    }
    catch (const boost::system::system_error &error) {
        fmt::print("Failed to open socket: {}\n", error.what());
        return EXIT_FAILURE;
    }
    catch (const std::exception &error) {
        fmt::print("Unknown error: {}\n", error.what());
        return EXIT_FAILURE;
    }

    // Allow sending of broadcast messages
    try {
        socket.set_option(asio::socket_base::broadcast{true});
    }
    catch (const boost::system::system_error &error) {
        fmt::print("Failed to set broadcast option for socket: {}\n", error.what());
        return EXIT_FAILURE;
    }
    catch (const std::exception &error) {
        fmt::print("Unknown error: {}\n", error.what());
        return EXIT_FAILURE;
    }

    asio::ip::udp::endpoint endpoint{asio::ip::network_v4{}.broadcast(), port};

    try {
        socket.send_to(asio::buffer(payload), endpoint);
    }
    catch (const boost::system::system_error &error) {
        fmt::print("Send error: {}\n", error.what());
        return EXIT_FAILURE;
    }
    catch (const std::exception &error) {
        fmt::print("Unknown error: {}\n", error.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
