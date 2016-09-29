// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef INSTALLER_PARTMAN_OPERATION_H
#define INSTALLER_PARTMAN_OPERATION_H

#include <QList>

#include "partman/device.h"

namespace installer {

enum class OperationType {
  Create,
  Delete,
  Format,
  MountPoint,
  Resize,
  Invalid,  // operation type not set
};

// Abstract class for operations.
class Operation {
 public:
  Operation(OperationType type,
            const Partition& partition_orig,
            const Partition& partition_new);
  ~Operation();

  OperationType type;
  Partition partition_orig;
  Partition partition_new;

  // Apply changes to disk. Returns operation status.
  // Note that this method shall be called in the background thread.
  bool applyToDisk() const;

  // Apply operation by updating partition list.
  void applyToVisual(PartitionList& partitions) const ;

 private:
  int findIndexNew(const PartitionList& partitions) const;
  int findIndexOriginal(const PartitionList& partitions) const;

  // For operations which do not change partition boundaries.
  void substitute(PartitionList& partitions) const;
};

typedef QList<Operation> OperationList;

}  // namespace installer

#endif  // INSTALLER_PARTMAN_OPERATION_H