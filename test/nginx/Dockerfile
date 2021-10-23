FROM debian:buster

MAINTAINER etakouer <etakouer@student.42nice.fr>

ARG NGINX_V

# update repository
RUN apt-get -y update && \
echo "deb http://nginx.org/packages/mainline/debian buster nginx" > /etc/apt/sources.list.d/nginx.list && \
cd /tmp && \
apt-get -y install wget && \
apt-get -y install gnupg2 && \
wget https://nginx.org/keys/nginx_signing.key && \
apt-key add nginx_signing.key && \
apt-get -y update

# install
RUN apt-get -y install nginx=$NGINX_V-1~buster && \
apt-get clean -y

EXPOSE 443

CMD ["nginx", "-g", "daemon off;"]
