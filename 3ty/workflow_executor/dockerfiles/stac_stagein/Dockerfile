FROM terradue/l1-binder:3.0

MAINTAINER Terradue S.r.l

RUN git clone https://gitlab.com/ellip/incubator/stage-in.git

RUN /opt/anaconda/bin/conda env create --file ${HOME}/stage-in/environment.yml && /opt/anaconda/bin/conda clean -a -y

RUN /opt/anaconda/envs/env_stagein/bin/python -m ipykernel install --name env_stagein

RUN cd ${HOME}/stage-in/ && /opt/anaconda/envs/env_stagein/bin/python setup.py install

ENV PREFIX /opt/anaconda/envs/env_stagein

ENV PATH="/opt/anaconda/envs/env_stagein/bin:${PATH}"

WORKDIR ${HOME}

#ENTRYPOINT ["/opt/anaconda/envs/env_stagein/bin/stage-in"]

#CMD ["--help"]
