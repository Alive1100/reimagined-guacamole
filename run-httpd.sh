#!/bin/bash
docker run -dit --rm -v /home/si/git/nar/htdocs:/usr/local/apache2/htdocs -p 443:443 nar-httpd
