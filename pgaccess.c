#include "pgaccess.h"
#include <libpq-fe.h>
#include <math.h>
//#include <windows.h>
static PGconn* connection = NULL;	/**< Access using getconn. */
static PGconn* putConnect = NULL;	/**< For accessing the put thread. */
static char* pid = NULL;	/**< Process id for accessing data base. */
static int quit = 0;	/**< Set to true when we need to start another cycle. */
static int appQuit = 0;		/**< Set to true when we need to exit the program. */
static pv* pvArray = NULL;	/**< Pvs that are being handled. */
static int pvArraySize = 0;	/**< Size of pvArray. */
const double LS_INIT_TIMEOUT = 20;
void quitConnection();

/**
* Get the shared connection.
*
* @return Shared pgsql connection.
*/
PGconn* getconn()
{
	PGresult* res;

	if(!connection)
	{
		/* open up the connection. */
	        connection = PQconnectdb("dbname=ls user=lsuser host=10.1.0.3");
		putConnect = PQconnectdb("dbname=ls user=lsuser host=10.1.0.3");

		if(PQstatus(connection) != CONNECTION_OK || PQstatus(putConnect) != CONNECTION_OK)
		{
			printf("Failed to connection to contrabass.\n");
			quitConnection();
			return NULL;
		}

		/** get the next pid. */
		res = PQexec(connection, "select epics.iniCAMonitor()");
		if(PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0)
		{
			pid = malloc(strlen(PQgetvalue(res, 0, 0)) + 1);	/* alocate enough memory. */
			strcpy(pid, PQgetvalue(res, 0, 0));	/* copy into our string. */
			pid[strlen(PQgetvalue(res, 0, 0))] = '\0';	/* ensure null termination. */
			PQclear(res);	/* clean up result. */
			printf("The pid is: %s\n", pid);


			/* Listen to putnotify command so we now when to call checkPutqueue. */
			PQexec(connection, "listen caputnotify");
		} else {
			printf("failed to get pid\n");
			/* set pid to an illegal value. */
			pid = malloc(sizeof(char) * 2);
			pid[0] = -1;
			pid[1] = 0;
		}
	}

	return connection;
}

/**
* Get the connection for the put thread.
*
* @return Pgsql connection.
*/
PGconn* getPutConnect()
{
	getconn();	/** This call will initialize the connection. */
	return putConnect;
}

void quitConnection()
{
	PGresult* res;
	const char* params[1];

	if(connection)
	{
		printf("Quiting\n\n\n\n");
		params[0] = pid;
		res = PQexecParams( connection, "select epics.checkpid($1::bigint)", 1, NULL, params, NULL, NULL, 1);
		if(PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			if(PQgetisnull(res, 0, 0))
			{
				/* app should quit now, database has a new slave. */
				printf("Got app quit\n");
				appQuit = 1;
			}
		} else {
			printf("checkpid failed status: %s message: %s\n", PQresStatus(PQresultStatus(res)), PQresultErrorMessage(res));
		}
		PQclear(res);
	}
	connection = NULL;
	quit = 1;
}

/**
* Check the return value to make sure it is legal.
* If the result was bad, clears res.
*
* @param res Result to check.
* @return 0 if everying is ok, 1 if it is time to exit.
*/
char checkReturn(PGresult* res, const char* funcName)
{
	int result = PQresultStatus(res);
	if(result == PGRES_NONFATAL_ERROR || result == PGRES_FATAL_ERROR)
	{
		printf("Return failed func: %s status: %s message: %s\n", funcName, PQresStatus(PQresultStatus(res)), PQresultErrorMessage(res));
		PQclear(res);	/* user will be exiting and not clean up their PGresult. */
		quitConnection();
		return 1;
	}

	return 0;
}

/**
* Updates the database.
*
* @param index Index of the process variable to update.
* @param value New value of the varaible.
*/
void ls_update(int index, const char* value)
{
	static char buffer[512];
	const char* params[3];
	PGresult* res;

	if(quit)
	{ return; }

	sprintf(buffer, "%i", index);

	params[0] = pid;
	params[1] = buffer;
	params[2] = value;

	if(strlen(value))
	{
		//printf("aselect epics.pvupdatevalue(%s, %s, %s)\n", params[0], params[1], params[2]);
		res = PQexecParams(putConnect, "select epics.pvupdatevalue($1::bigint,$2::int,$3)", 3, NULL, params, NULL, NULL, 0);
	} else {
		//printf("bselect epics.pvupdatevalue(%s, %s, null)\n", params[0], params[1]);
		res = PQexecParams(putConnect, "select epics.pvupdatevalue($1::bigint,$2::int,null)", 2, NULL, params, NULL, NULL, 0);
	}
	
	
	if(checkReturn(res, "epics.pvupdatevalue"))
	{ return; }
	PQclear(res);	/* not interested in the result. */
}

/**
* Convert a dbr value to a double.
*
* @param value Value array.
* @param type Type of value.
* @param index Index in the array.
* @return value[index] as a double or 0 if unable to convert.
*/
double val2d(const void* value, unsigned int type, int index)
{
	unsigned int base_type;
	void* value_ptr;

	if(!dbr_type_is_valid(type))
	{
		return 0;
	}
	
	base_type = type % (LAST_TYPE+1);
	value_ptr = dbr_value_ptr(value, type);
	switch(base_type)
	{
	case DBR_FLOAT:
		return ((dbr_float_t*)value_ptr)[index];
	case DBR_DOUBLE:
		return ((dbr_double_t*)value_ptr)[index];
	case DBR_CHAR:
		return ((dbr_char_t*)value_ptr)[index];
	case DBR_INT:
		return ((dbr_int_t*)value_ptr)[index];
	case DBR_LONG:
		return ((dbr_long_t*)value_ptr)[index];
	default:
		/* don't know how to convert to a double, return 0 as default. */
		return 0.0;
	}
}

void ls_updatepv(pv* mpv)
{
	double curVal;
	double delta;

	if(quit)
	{ return; }

	if(mpv->delta > 0)
	{
		/* check to see if we need to update. */
		curVal = val2d(mpv->value, mpv->dbrType, 0);
		if(mpv->lastValueInit)
		{
			delta = fabs(curVal - mpv->lastValue);
			if(delta < mpv->delta)
			{
				/* The value is just noise. */
				return;
			}
			/* Not noise, we have a new last value. */
			mpv->lastValue = curVal;
		} else {
			/* Initialize lastValue. */
			mpv->lastValue = curVal;
			mpv->lastValueInit = 1;
		}
	}

	/* update the value in the data base. */
	ls_update( mpv->index, val2str(mpv->value, mpv->dbrType, 0));
}
void initialize_pgdb()
{
	PGresult* res;
	int i, value, returncode;
	const char* params[1];
        int sz;

	if(quit)
	{ return; }
	getconn();
	params[0] = pid;
	printf("pid: %s\n", params[0]);
	res = PQexecParams(getconn(), "select index, name, delta from epics.getpvnames($1::bigint)", 1, NULL, params, NULL, NULL, 0);
	if(checkReturn(res, "epics.getpvnames"))
	{ return; }

	/* Create pvArray. */
	pvArray = calloc(PQntuples(res), sizeof(pv));
	pvArraySize = PQntuples(res);
	for(i = 0; i < pvArraySize; i++)
	{
	  value = atoi(PQgetvalue(res, i, 0));
	  if(value < 0 || value >= pvArraySize)
	    {
	      printf("index %i is illegal", value);
	      quitConnection();
	      return;
	    }
	  
	  pvArray[value].index = value;

	  sz = strlen( PQgetvalue( res, i, 1)) + 1;
	  pvArray[value].name = malloc(sz);
	  strcpy(pvArray[value].name, PQgetvalue(res, i, 1));
	  pvArray[value].name[sz-1] = 0;
	  pvArray[value].delta = atof(PQgetvalue(res, i, 2));
	  pvArray[value].lastValueInit = 0;
	  pvArray[value].onceConnected = 0;
	  
	  printf("pv(%i): %s\n", value, pvArray[value].name);
	}

	/* Initialize pvArray and wait for them to connect. */
	returncode = create_pvs(pvArray, pvArraySize, connection_handler);
	if(returncode)
	{
		printf("Failed to create pvs(%i)", returncode);
		quitConnection();
	}
	printf("Before pend_event\n");
	ca_pend_event(LS_INIT_TIMEOUT);
	printf("After pend_event\n");
	
	/* Make sure pvs have connected successfuly, if they have not log the error. */
	for(i = 0; i < pvArraySize; i++)
	{
		if(!pvArray[i].onceConnected)
		{
			printf("Failed to initialize pv(%i): %s\n", i, pvArray[i].name);
		}
	}
}

void ls_checkRunqueue()
{
	PGresult* res;
	const char* params[1];
	char* value;
	int i, index, ret;
	
	if(quit)
	{ return; }

	params[0] = pid;
	res = PQexecParams(getconn(), "select index, value from epics.popputqueue($1::bigint)", 1, NULL, params, NULL, NULL, 0);
	if(checkReturn(res, "epics.poprunqueue"))
	{ return; }

	/* Send each command to the motors. */
	for(i = 0; i < PQntuples(res); i++)
	{
		index = atoi(PQgetvalue(res, i, 0));
		if(index >= pvArraySize)
		{
			/* Quit if the index is illeal the pvArray has been changed on the server side. */
			quitConnection();
			return;
		}
		value = PQgetvalue(res, i, 1);
		printf("moving %s (index %i got %s) to %s\n", pvArray[index].name, index, PQgetvalue(res, i, 0), value);
		ret = ca_put(DBR_STRING, pvArray[index].chid, value);
		if( ret != ECA_NORMAL)
		{
			printf("caput failed on type %s because of %i\n", pvArray[i].name, ret);
		}
	}
	ca_flush_io();
}

void ls_run() {
  PGnotify* notify;
  fd_set readfds, writefds, execptfds;
  struct timeval st;

  initialize_pgdb();	/* Initialize pvs. */
  while(!quit) {
    //ca_pend_event(LS_PEND_TIMEOUT);	/* Monitor events. */
    if(quit)
      { break; }

    /* Service runqueue if nessisary. */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&execptfds);
    FD_SET(PQsocket(getconn()), &readfds);
    st.tv_usec=500000;

    select(1, &readfds, &writefds, &execptfds, &st);
    PQconsumeInput(getconn());
    ls_checkRunqueue();
    notify = PQnotifies(getconn());	
    if(notify) {
      printf( "In Notify\n");
      PQfreemem(notify);
      while((notify = PQnotifies(getconn()))) { PQfreemem(notify); }
      ls_checkRunqueue();
    }
  }
}

int main()
{
	int result;

	printf("Started\n");
	enumAsNr=1;  // Probably not where you want this, Max, but we need it somewhere to keep enums from being returned as strings
	while(!appQuit)
	{
		quit = appQuit;	/* Starting over again, clear quit. */
		result = ca_context_create(ca_enable_preemptive_callback);
		if (result == ECA_NORMAL)
		{
			printf("New run\n");
			ls_run();
			ca_context_destroy();	/* stop backround threads. */
			/* Closes and dealocates connection. */
			PQfinish(connection);
			PQfinish(putConnect);
			/** free memory. */
			if(pid)
			{
				free(pid);
				pid = NULL;
			}
			if(pvArray)
			{
				free(pvArray);
				pvArray = NULL;
			}
		} else {
			fprintf(stderr, "CA error %s occurred while trying to start channel access.\n", ca_message(result));
		}
	}

	return 0;
}
