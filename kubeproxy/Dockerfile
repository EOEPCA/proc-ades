FROM alpine:3.6
ADD https://storage.googleapis.com/kubernetes-release/release/v1.7.4/bin/linux/amd64/kubectl /usr/local/bin/kubectl
RUN chmod +x /usr/local/bin/kubectl
ADD kubectl-proxy.sh /kubectl-proxy.sh 
RUN chmod +x /kubectl-proxy.sh 
EXPOSE 8001
ENTRYPOINT ["/bin/sh", "-c", "/kubectl-proxy.sh"]