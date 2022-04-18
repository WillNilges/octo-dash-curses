#!/bin/bash
podman run --rm -it -v$(pwd):/odc --name=odc-builder odc-builder
