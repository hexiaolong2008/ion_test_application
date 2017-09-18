#include <stdio.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#include <ion.h>

#define MAX_HEAPS 32
#define MAX_LEN 1024*16

int main(int argc, char ** argv)
{
	int ion_fd = 0;
	int err, i;
	struct ion_heap_data heaps_data[MAX_HEAPS];
	struct ion_heap_query query;

	ion_fd = open("/dev/ion0", O_RDONLY);
	if(!ion_fd) {
		printf("Failed to open ION device - %s\n", strerror(errno));
		return -errno;
	}

	memset(&query, 0, sizeof(query));
	query.cnt = MAX_HEAPS;
	query.heaps = &heaps_data;
	err = ioctl(ion_fd, ION_IOC_HEAP_QUERY, &query);
	close(ion_fd);
	if (err) {
		printf("Can't get ION heaps data %s\n", strerror(errno));
		goto out;
	}

	printf("Heap name \t\ttype \tid \talloc \tmmap\n");
	for (i = 0; i < query.cnt; i++) {
		int fd;
		struct ion_allocation_data alloc;
		void *buffer = NULL;
		char filename[32];

		sprintf(filename, "/dev/ion%d", heaps_data[i].heap_id);
		ion_fd = open(filename, O_RDONLY);

		memset(&alloc, 0, sizeof(alloc));
		alloc.len = MAX_LEN;
		alloc.heap_id_mask = 1 << heaps_data[i].heap_id;
		err = ioctl(ion_fd, ION_IOC_ALLOC, &alloc);
		if (!err)
			buffer = mmap(0, MAX_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, alloc.fd, 0);

		printf("%s \t\t%d \t%d \t%s \t%s\n",
			heaps_data[i].name,
			heaps_data[i].type ,
			heaps_data[i].heap_id,
			err ? "KO" : "OK",
			buffer ? "OK": "KO");
		close(fd);
		close(ion_fd);
	}

out:
	return err;
}
