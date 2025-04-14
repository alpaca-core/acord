ARG CMAKEVERSION=3.31.2
ARG BUILD_TYPE=release

FROM nvidia/cuda:12.8.1-devel-ubuntu24.04 AS dependencies
ENV WORKDIR=/usr/local/acord
WORKDIR ${WORKDIR}
ARG BUILD_TYPE

RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential cmake ninja-build git && \
    rm -rf /var/lib/apt/lists/*


FROM dependencies AS build
COPY . .
RUN cmake --preset ${BUILD_TYPE}-demo -DACORD_BUILD_TESTS=OFF -DACORD_BUILD_EXAMPLES=OFF -G Ninja && \
    ninja -C out/build/${BUILD_TYPE}-demo


FROM nvidia/cuda:12.8.1-runtime-ubuntu24.04 AS final
ENV WORKDIR=/usr/local/acord
WORKDIR ${WORKDIR}
ARG BUILD_TYPE
ENV BUILD_TYPE=${BUILD_TYPE}
ENV PLUGINSDIR=${WORKDIR}/out/build/${BUILD_TYPE}-demo/_ac-plugins

RUN apt-get update && \
    apt-get install -y --no-install-recommends build-essential && \
    rm -rf /var/lib/apt/lists/*


# Copy the full directory structure in order to find the plugins
# TODO: Check how to use rpath instead of copy the full directory structure
# COPY --from=build /usr/local/acord/install/ /usr/local/acord/

COPY --from=build ${WORKDIR}/out/build/${BUILD_TYPE}-demo/bin ${WORKDIR}/out/build/${BUILD_TYPE}-demo/bin

RUN mkdir -p ${PLUGINSDIR}/lib/ac-local

COPY --from=build ${PLUGINSDIR}/ilib-llama.cpp-0.1.1-default/bin ${PLUGINSDIR}/ilib-llama.cpp-0.1.1-default/bin
RUN cp ${PLUGINSDIR}/ilib-llama.cpp-0.1.1-default/bin/aclp-llama.so ${PLUGINSDIR}/lib/ac-local/aclp-llama.so

COPY --from=build ${PLUGINSDIR}/ilib-sd.cpp-0.1.1-default/bin ${PLUGINSDIR}//ilib-sd.cpp-0.1.1-default/bin
RUN cp ${PLUGINSDIR}/ilib-sd.cpp-0.1.1-default/bin/aclp-sd.so ${PLUGINSDIR}/lib/ac-local/aclp-sd.so

COPY --from=build ${PLUGINSDIR}/ilib-foo-0.1.3-default/bin ${PLUGINSDIR}/ilib-foo-0.1.3-default/bin
RUN cp ${PLUGINSDIR}/ilib-foo-0.1.3-default/bin/aclp-foo.so ${PLUGINSDIR}/lib/ac-local/aclp-foo.so

EXPOSE 7654
CMD ["sh", "-c", "./out/build/${BUILD_TYPE}-demo/bin/acord --public"]
