# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.202.5/containers/ubuntu/.devcontainer/base.Dockerfile

# [Choice] Ubuntu version (use hirsuite or bionic on local arm64/Apple Silicon): hirsute, focal, bionic
FROM gitpod/workspace-full
USER root
RUN apt-get update && apt-get install -y qemu-system-x86 linux-image-generic isc-dhcp-client supervisor libvirt-daemon-system


RUN echo "9p\n9pnet\n9pnet_virtio" >> /etc/initramfs-tools/modules
COPY scripts/curl /usr/share/initramfs-tools/hooks
COPY scripts/mount_source /usr/share/initramfs-tools/scripts/local-premount
COPY scripts/start_net /usr/share/initramfs-tools/scripts/local-premount
RUN update-initramfs -u
RUN cp -L /boot/vmlinuz /home/gitpod/ \
    && chown gitpod:gitpod /home/gitpod/vmlinuz \
    && cp -L /boot/initrd.img /home/gitpod/ \
    && chown gitpod:gitpod /home/gitpod/initrd.img

RUN mkdir -p /var/run/user/gitpod && chown gitpod:gitpod /var/run/user/gitpod && chmod 700 /var/run/user/gitpod

USER gitpod
ENV XDG_RUNTIME_DIR=/var/run/user/gitpod
RUN echo export LIBVIRT_DEFAULT_URI="qemu:///session" >> $HOME/.profile
COPY --chown=gitpod:gitpod supervisord.conf $HOME/supervisord.conf
COPY --chown=gitpod:gitpod vm.xml $HOME/vm.xml

# [Optional] Uncomment this section to install additional OS packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>


