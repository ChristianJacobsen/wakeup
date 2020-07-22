#!/usr/bin/env sh

# Fetch the current artifact
artifact pull workflow build/wakeup

# Create a tar-ball
tar zcf "wakeup-$(build/wakeup --version).tar.gz" build/wakeup

# Publish the final tar-balled binary
artifact push project "wakeup-$(build/wakeup --version).tar.gz"
