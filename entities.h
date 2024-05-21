#ifndef ENTITIES_H
#define ENTITIES_H
enum orderStatus {orderStatus_created, orderStatus_inProcess, orderStatus_completed, orderStatus_unclaimed, orderStatus_doesNotExist};

char *orderStatusName(enum orderStatus in) {
  switch (in) {
    case orderStatus_created:
    return "created";
    case orderStatus_inProcess:
    return "inProcess";
    case orderStatus_completed:
    return "completed";
    case orderStatus_unclaimed:
    return "unclaimed";
    case orderStatus_doesNotExist:
    return "does not exist";
    default: exit(1);
  }
}
#endif
