#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[]) {

	int grandparentID = (int) getpid(); //pid for output
	int rc = fork();
	if (rc < 0) { //fork failed
		fprintf(stderr, "fork failed\n");
		exit(1);
	} else if (rc == 0) { //fork successful
		int parentID = (int) getpid(); //get pid for output

		rc = fork(); //fork to grandchild
		if (rc < 0) { //fork failed
			printf("fork failed\n");
			exit(1);
		} else if (rc == 0) { //grandchild fork successful
			int childID = (int) getpid();
			printf(
					"My process ID is (pid:%d), my parents ID is (pid:%d), and my grandparents ID is (pid:%d)",
					childID, parentID, grandparentID);
			return 0;

		}

	}
}
