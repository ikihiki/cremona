# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.202.5/containers/ubuntu/.devcontainer/base.Dockerfile

# [Choice] Ubuntu version (use hirsuite or bionic on local arm64/Apple Silicon): hirsute, focal, bionic
FROM gitpod/workspace-full
USER root
RUN apt-get update && apt-get install -y qemu-system-x86 linux-image-generic isc-dhcp-client

RUN echo "9p\n9pnet\n9pnet_virtio" >> /etc/initramfs-tools/modules
COPY scripts/curl /usr/share/initramfs-tools/hooks
COPY scripts/mount_source /usr/share/initramfs-tools/scripts/local-premount
COPY scripts/start_net /usr/share/initramfs-tools/scripts/local-premount
RUN update-initramfs -u

USER gitpod
# [Optional] Uncomment this section to install additional OS packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>


