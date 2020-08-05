FROM proc-comm-zoo:1.0


#RUN git clone https://github.com/EOEPCA/proc-ades.git && git check
















#
#
#COPY 3ty/workflow_executor /usr/local/workflow_executor
#RUN curl -O https://repo.anaconda.com/miniconda/Miniconda3-latest-Linux-x86_64.sh && /bin/bash Miniconda3-latest-Linux-x86_64.sh -b
#ENV PATH="/opt/app-root/src/miniconda3/bin:$PATH"
#RUN conda install -c conda-forge python-kubernetes click
#RUN cd /usr/local/workflow_executor/ && python setup.py install
#
#
#
#COPY assets/main.cfg /opt/t2service/main.cfg
#COPY assets/oas.cfg /opt/t2service/oas.cfg
#
##COPY src/zoo /tmp/zoo
##RUN cd /tmp/zoo && make && make install && rm -fvR /tmp/zoo && chmod +x /opt/t2scripts/entrypoint.sh
#RUN chmod +x /opt/t2scripts/entrypoint.sh
#
#COPY assets/workflowwxecutorconfig.json /opt/t2config/workflowwxecutorconfig.json
#COPY src/templates/template.cpp /opt/t2template/template.cpp
#COPY src/templates/Makefile /opt/t2template/Makefile
#COPY src/deployundeploy/zoo/build/libepcatransactional.zo /opt/t2service/
#COPY src/deployundeploy/zoo/*.zcfg /opt/t2service/
#COPY src/templates/libinterface.so /opt/t2libs/libinterface.so
#COPY build/3ty/proc-comm-lib-ows-1.04/libeoepcaows.so /opt/t2libs/
##COPY build/3ty/proc-comm-lib-argo/proc-comm-lib-argo-build/libeoepcaargo.so /opt/t2service/libeoepcaargo.so
#COPY build/libworkflow_executor.so /opt/t2service/libworkflow_executor.so
#RUN mkdir -p /opt/zooservices_user && chown 48:48 /opt/zooservices_user
#COPY assets/scripts/prepareUserSpace.sh /opt/t2scripts/prepareUserSpace.sh
#RUN chmod +x /opt/t2scripts/prepareUserSpace.sh
#
#COPY assets/config /opt/t2config/kubeconfig
#RUN chown 48:48 /opt/t2config/kubeconfig
#
##sudo docker run --rm  -d --name zoo -p 7777:80    proc-ades:1.0
##sudo docker run --rm  -d --name zoo -p 7777:80  -v $PWD:/project  proc-ades:1.0
##alias ll='ls -ltr'
## docker exec -ti -e COLUMNS="`tput cols`" -e LINES="`tput lines`" zoo  bash
#
#RUN echo "alias ll='ls -ltr'" >> $HOME/.bashrc

