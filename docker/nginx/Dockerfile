FROM nginx:1.20.1

RUN apt-get -y update && \
apt-get -y install wget gnupg2 vim tmux procps spawn-fcgi fcgiwrap && \
apt-get clean -y

EXPOSE 80

COPY ./entry.sh /

ENTRYPOINT ["./entry.sh"]

CMD ["nginx","-g","daemon off;"]
