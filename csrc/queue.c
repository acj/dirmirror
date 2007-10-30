#include "queue.h"
#include <malloc.h>
#include <string.h>

struct DMQueue
{
	unsigned int count;
	unsigned int max_count;
	char **buf;
};
typedef struct DMQueue DMQueue;

void dmqueue_init(DMQueue *dmq)
{
	dmq->count = 0;
	dmq->max_count = (int)DM_START_SIZE;
	dmq->buf = (char **)calloc(DM_START_SIZE, sizeof(char *));
}

unsigned int dmqueue_count(DMQueue *dmq)
{
	return dmq->count;
}

void dmqueue_grow(DMQueue *dmq)
{
	unsigned int new_max = dmq->max_count + 100;

	char **new_buf = (char **)calloc(new_max, sizeof(char *));
	memcpy(new_buf, dmq->buf, dmq->max_count * sizeof(char *));

	free(dmq->buf);
	dmq->buf = new_buf;
}

unsigned int dmqueue_max(DMQueue *dmq)
{
	return dmq->max_count;
}

void dmqueue_push_dir(DMQueue *dmq, char *dir)
{
	if (dmqueue_count(dmq) == dmqueue_max(dmq))
	{
		dmqueue_grow(dmq);
	}
	
	dmq->buf[dmq->count] = strdup(dir);
}

