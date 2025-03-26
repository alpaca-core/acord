ARG CMAKEVERSION=3.31.2
ARG BUILD_TYPE=debug

FROM ubuntu:latest AS base
WORKDIR /usr/local/acord

FROM base AS build
ARG BUILD_TYPE

RUN apt-get update && \
    apt-get install -y build-essential cmake ninja-build git

COPY . .
RUN mkdir -p out/build &&\
    cd out/build && \
    cmake --preset ${BUILD_TYPE}-demo -G Ninja ../.. && \
    ninja -C ${BUILD_TYPE}-demo

FROM base AS final
ARG WORKDIR=/usr/local/acord

# Copy the full directory structure in order to find the plugins
# TODO: Check how to use rpath instead of copy the full directory structure
COPY --from=build ${WORKDIR}/out/build/debug-demo/bin ${WORKDIR}/out/build/debug-demo/bin
COPY --from=build ${WORKDIR}/out/build/debug-demo/_ac-plugins/lib/ac-local ${WORKDIR}/out/build/debug-demo/_ac-plugins/lib/ac-local

EXPOSE 7654
CMD ["./out/build/debug-demo/bin/acord"]


