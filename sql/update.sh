#!/bin/sh

psql_server=localhost
echo "On which server do you intend to apply your changes? [localhost]"
read response
if [ -n "$response" ]; then
    psql_server=$response
fi
psql_db=ls
psql_user=$(echo $USERNAME | sed 's/-local//g')
echo "Understood, updating $psql_server, db $psql_db, as user $psql_user."

# All of the plpython3u functions are written in a .py file; this is to preserve
# syntax highlighting in emacs and IDEs. The SQL code to create the handler is
# embedded in the python file in the form of comments starting with "!SQL!"
# (no quotes).
tmpfile=$(mktemp -p /tmp plpython3u-XXX.sql)
for file in $(find . -name '*.py'); do
    sed 's/#\+!SQL!#*\ *//g' $file >> $tmpfile
done

# Execute the plain-old SQL files first, as they create the schemas,
# and add the python functions last.
cat $(find . -name '*.sql') $tmpfile | \
    psql -W -h $psql_server -d $psql_db -U $psql_user
status=$?
if [ $status -eq 0 ]; then
    rm $tmpfile
else
    echo "psql failed with status $status, check one of the .sql files or" \
	 "$tmpfile in the event of a syntax error."
    exit $status
fi
