FROM nginx:1.20.1

RUN apt-get -y update && \
apt-get -y install wget gnupg2 vim tmux procps && \
apt-get clean -y

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
