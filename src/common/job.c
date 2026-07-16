#include "job.h"

const char* getStatusString(JobStatus status) {
    switch (status) {
        case QUEUED: return "QUEUED";
        case RUNNING: return "RUNNING";
        case COMPLETED: return "COMPLETED";
        case CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}
