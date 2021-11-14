FROM nginx:1.20.1

RUN apt-get -y update && \
apt-get -y install wget gnupg2 vim tmux procps spawn-fcgi fcgiwrap ncat git socat python python3 python3-pip && \
apt-get clean -y && \
pip3 install requests

COPY ./entry.sh /

ENTRYPOINT ["./entry.sh"]

EXPOSE 4242
EXPOSE 4343

CMD ["nginx","-g","daemon off;"]
