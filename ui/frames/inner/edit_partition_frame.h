// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#ifndef DEEPIN_INSTALLER_REBORN_UI_FRAMES_INNER_EDIT_PARTITION_FRAME_H
#define DEEPIN_INSTALLER_REBORN_UI_FRAMES_INNER_EDIT_PARTITION_FRAME_H

#include <QFrame>
#include <ui/widgets/nav_button.h>
#include <ui/widgets/table_combo_box.h>

namespace ui {

class NavButton;
class PartitionDelegate;
class TableComboBox;

class EditPartitionFrame : public QFrame {
  Q_OBJECT

 public:
  EditPartitionFrame(PartitionDelegate* delegate, QWidget* parent = nullptr);

  // Set partition information at |partition_path|.
  void setPath(const QString& partition_path);

 signals:
  void finished();

 private:
  void initConnections();
  void initUI();

  TableComboBox* fs_box_ = nullptr;
  TableComboBox* mount_point_box_ = nullptr;
  NavButton* cancel_button_ = nullptr;
  NavButton* ok_button_ = nullptr;

  PartitionDelegate* delegate_ = nullptr;

 private slots:
  void onOkButtonClicked();
};

}  // namespace ui

#endif  // DEEPIN_INSTALLER_REBORN_UI_FRAMES_INNER_EDIT_PARTITION_FRAME_H
