# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.202.5/containers/ubuntu/.devcontainer/base.Dockerfile

# [Choice] Ubuntu version (use hirsuite or bionic on local arm64/Apple Silicon): hirsute, focal, bionic
FROM gitpod/workspace-full
USER root
RUN apt-get update \
    && apt install -y curl gnupg \
    && curl -fsSL https://bazel.build/bazel-release.pub.gpg | gpg --dearmor > bazel.gpg \
    && mv bazel.gpg /etc/apt/trusted.gpg.d/ \
    && echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" > /etc/apt/sources.list.d/bazel.list \
    && apt-get update \
    && apt-get install -y qemu-system-x86 linux-image-generic linux-headers-generic isc-dhcp-client supervisor libvirt-daemon-system clangd bazel \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

COPY scripts/curl /usr/share/initramfs-tools/hooks
COPY scripts/mount_source /usr/share/initramfs-tools/scripts/local-premount
COPY scripts/start_net /usr/share/initramfs-tools/scripts/local-premount
RUN echo "9p\n9pnet\n9pnet_virtio" >> /etc/initramfs-tools/modules \
    && update-initramfs -u \
    && cp -L /boot/vmlinuz /home/gitpod/ \
    && chown gitpod:gitpod /home/gitpod/vmlinuz \
    && cp -L /boot/initrd.img /home/gitpod/ \
    && chown gitpod:gitpod /home/gitpod/initrd.img \
    && mkdir -p /var/run/user/gitpod && chown gitpod:gitpod /var/run/user/gitpod && chmod 700 /var/run/user/gitpod

USER gitpod
ENV XDG_RUNTIME_DIR=/var/run/user/gitpod
RUN echo export LIBVIRT_DEFAULT_URI="qemu:///session" >> $HOME/.profile
COPY --chown=gitpod:gitpod supervisord.conf $HOME/supervisord.conf
COPY --chown=gitpod:gitpod vm.xml $HOME/vm.xml
 # bazel-watcher
RUN git clone https://github.com/bazelbuild/bazel-watcher.git \
    && cd bazel-watcher \
    && bazel build //ibazel \
    && echo 'export PATH=$PATH:/home/gitpod/bazel-watcher/bazel-bin/ibazel/linux_amd64_stripped' >> $HOME/.profile

# [Optional] Uncomment this section to install additional OS packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>


