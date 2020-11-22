#!/bin/bash
docker run -dit --rm -v /home/ec2-user/git/nar/htdocs:/usr/local/apache2/htdocs -p 443:443 nar-httpd
