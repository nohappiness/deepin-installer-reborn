// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef DEEPIN_INSTALLER_REBORN_UI_FRAMES_INNER_EDIT_PARTITION_FRAME_H
#define DEEPIN_INSTALLER_REBORN_UI_FRAMES_INNER_EDIT_PARTITION_FRAME_H

#include <QFrame>
#include <ui/widgets/table_item_label.h>

class QCheckBox;

#include "partman/partition.h"

namespace ui {

class FsModel;
class MountPointModel;
class NavButton;
class PartitionDelegate;
class TableComboBox;
class TableItemLabel;

class EditPartitionFrame : public QFrame {
  Q_OBJECT

 public:
  EditPartitionFrame(PartitionDelegate* delegate, QWidget* parent = nullptr);

  // Set partition information at |partition_path|.
  void setPartition(const partman::Partition& partition);

 signals:
  void finished();

 private:
  void initConnections();
  void initUI();

  TableComboBox* fs_box_ = nullptr;
  TableComboBox* mount_point_box_ = nullptr;
  TableItemLabel* mount_point_label_ = nullptr;
  QCheckBox* format_check_box_ = nullptr;
  TableItemLabel* format_label_ = nullptr;
  NavButton* cancel_button_ = nullptr;
  NavButton* ok_button_ = nullptr;

  PartitionDelegate* delegate_ = nullptr;
  FsModel* fs_model_ = nullptr;
  MountPointModel* mount_point_model_ = nullptr;
  partman::Partition partition_;

 private slots:
  void onFsChanged(int index);
  void onOkButtonClicked();
};

}  // namespace ui

#endif  // DEEPIN_INSTALLER_REBORN_UI_FRAMES_INNER_EDIT_PARTITION_FRAME_H
