#!/bin/bash
podman run --rm -it -v./:/odc --name=odc-builder odc-builder
