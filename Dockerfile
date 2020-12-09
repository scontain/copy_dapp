FROM sconecuratedimages/muslgcc

COPY copy_files.c  /

# compile with vanilla gcc
RUN gcc -Wall copy_files.c -o /copy_files

FROM sconecuratedimages/dapps:copy_dapp_runtime

COPY --from=0 /copy_files /application

RUN apk add curl bash unzip zip

ENTRYPOINT ["/application.sh"]
