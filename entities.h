#ifndef ENTITIES_H
#define ENTITIES_H
enum orderStatus {orderStatus_created, orderStatus_inProcess, orderStatus_completed};

char *orderStatusName(enum orderStatus in) {
  switch (in) {
    case orderStatus_created:
    return "created";
    case orderStatus_inProcess:
    return "inProcess";
    case orderStatus_completed:
    return "completed";
    default: exit(2);
  }
}
#endif
