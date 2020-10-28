FROM proc-comm-zoo:1.0

COPY 3ty/workflow_executor /usr/local/workflow_executor

RUN curl -O https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh && /bin/bash Miniconda3-latest-Linux-x86_64.sh -b
ENV PATH="/opt/app-root/src/miniconda3/bin:$PATH"
RUN conda config --add channels eoepca
RUN conda config --add channels conda-forge
RUN conda install python-kubernetes click fastapi uvicorn cwl-wrapper
RUN cd /usr/local/workflow_executor/ && python setup.py install

WORKDIR /

#RUN git clone 'https://github.com/EOEPCA/proc-ades.git' /project
COPY . /project

WORKDIR /project
#RUN  git checkout 'develop' && mkdir build && cd build && cmake3 -DCMAKE_BUILD_TYPE=release -G "CodeBlocks - Unix Makefiles" ..
RUN  mkdir -p build && cd build && cmake3 -DCMAKE_BUILD_TYPE=release -G "CodeBlocks - Unix Makefiles" ..

WORKDIR /project/build/
RUN make eoepcaows workflow_executor && mkdir -p /project/zooservice

WORKDIR /project/zooservice
RUN make -C ../src/deployundeploy/zoo/
RUN make -C ../src/templates interface

WORKDIR /project


COPY assets/main.cfg /opt/t2service/main.cfg
COPY assets/oas.cfg /opt/t2service/oas.cfg

#COPY src/zoo /tmp/zoo
#RUN cd /tmp/zoo && make && make install && rm -fvR /tmp/zoo && chmod +x /opt/t2scripts/entrypoint.sh
COPY assets/scripts/entrypoint.sh /opt/t2scripts/entrypoint.sh
RUN chmod +x /opt/t2scripts/entrypoint.sh

COPY assets/workflowwxecutorconfig.json /opt/t2config/workflowwxecutorconfig.json
COPY src/templates/template.cpp /opt/t2template/template.cpp
COPY src/templates/Makefile /opt/t2template/Makefile
RUN cp /project/src/deployundeploy/zoo/build/libepcatransactional.zo /opt/t2service/
COPY src/deployundeploy/zoo/*.zcfg /opt/t2service/
RUN mkdir -p /opt/t2libs && cp /project/src/templates/libinterface.so /opt/t2libs/libinterface.so
RUN cp /project/build/3ty/proc-comm-lib-ows-1.04/libeoepcaows.so /opt/t2libs/

RUN cp /project/build/libworkflow_executor.so /opt/t2service/libworkflow_executor.so
RUN mkdir -p /opt/zooservices_user && chown 48:48 /opt/zooservices_user
COPY assets/scripts/prepareUserSpace.sh /opt/t2scripts/prepareUserSpace.sh
COPY assets/scripts/removeservice.sh /opt/t2scripts/removeservice.sh
RUN chmod +x /opt/t2scripts/prepareUserSpace.sh /opt/t2scripts/removeservice.sh

RUN echo "alias ll='ls -ltr'" >> $HOME/.bashrc
RUN yum install mlocate -y

EXPOSE 80

