ARG CMAKEVERSION=3.31.2
ARG BUILD_TYPE=debug

FROM ubuntu:latest AS base
WORKDIR /usr/local/acord

FROM base AS build
ARG BUILD_TYPE

RUN apt-get update && \
    apt-get install -y build-essential cmake ninja-build git

COPY . .
RUN cmake --preset ${BUILD_TYPE}-demo -G Ninja && \
    ninja -C out/build/${BUILD_TYPE}-demo

FROM base AS final
ARG WORKDIR=/usr/local/acord
ARG PLUGINSDIR=${WORKDIR}/out/build/debug-demo/_ac-plugins

RUN apt-get update && \
    apt-get install -y build-essential

# Copy the full directory structure in order to find the plugins
# TODO: Check how to use rpath instead of copy the full directory structure
COPY --from=build ${WORKDIR}/out/build/debug-demo/bin ${WORKDIR}/out/build/debug-demo/bin

RUN mkdir -p ${PLUGINSDIR}/lib/ac-local

COPY --from=build ${PLUGINSDIR}/ilib-llama.cpp-0.1.1-default/bin ${PLUGINSDIR}/ilib-llama.cpp-0.1.1-default/bin
RUN cp ${PLUGINSDIR}/ilib-llama.cpp-0.1.1-default/bin/aclp-llama.so ${PLUGINSDIR}/lib/ac-local/aclp-llama.so

COPY --from=build ${PLUGINSDIR}//ilib-sd.cpp-0.1.1-default/bin ${PLUGINSDIR}//ilib-sd.cpp-0.1.1-default/bin
RUN cp ${PLUGINSDIR}/ilib-sd.cpp-0.1.1-default/bin/aclp-sd.so ${PLUGINSDIR}/lib/ac-local/aclp-sd.so

COPY --from=build ${PLUGINSDIR}/ilib-foo-0.1.3-default/bin ${PLUGINSDIR}/ilib-foo-0.1.3-default/bin
RUN cp ${PLUGINSDIR}/ilib-foo-0.1.3-default/bin/aclp-foo.so ${PLUGINSDIR}/lib/ac-local/aclp-foo.so

EXPOSE 7654
CMD ["./out/build/debug-demo/bin/acord", "--all"]
