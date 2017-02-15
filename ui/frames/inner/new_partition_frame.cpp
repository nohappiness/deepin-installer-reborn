// Copyright (c) 2016 Deepin Ltd. All rights reserved.
// Use of this source is governed by General Public License that can be found
// in the LICENSE file.

#include "ui/frames/inner/new_partition_frame.h"

#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QStandardItemModel>

#include "base/file_util.h"
#include "service/settings_manager.h"
#include "service/settings_name.h"
#include "ui/frames/consts.h"
#include "ui/delegates/partition_delegate.h"
#include "ui/delegates/partition_util.h"
#include "ui/models/fs_model.h"
#include "ui/models/mount_point_model.h"
#include "ui/models/partition_type_model.h"
#include "ui/widgets/comment_label.h"
#include "ui/widgets/nav_button.h"
#include "ui/widgets/partition_size_slider.h"
#include "ui/widgets/table_combo_box.h"
#include "ui/widgets/title_label.h"

namespace installer {

namespace {

// Minimum size of new partition is 100 Mib.
const qint64 kMinimumPartitionSize = 100 * kMebiByte;

const int kContentSpacing = 15;

}  // namespace

NewPartitionFrame::NewPartitionFrame(PartitionDelegate* delegate,
                                     QWidget* parent)
    : QFrame(parent),
      delegate_(delegate),
      partition_() {
  this->setObjectName("new_partition_frame");

  this->initUI();
  this->initConnections();
}

void NewPartitionFrame::setPartition(const Partition& partition) {
  partition_ = partition;

  const bool primary_ok = delegate_->canAddPrimary(partition);
  const bool logical_ok = delegate_->canAddLogical(partition);
  if (!primary_ok && !logical_ok) {
    qCritical() << "No more partition available!";
  }
  type_model_->setPrimaryVisible(primary_ok);
  type_model_->setLogicalVisible(logical_ok);
  // Select logical partition is available.
  if (logical_ok) {
    type_box_->setCurrentIndex(type_model_->getLogicalIndex());
  } else if (primary_ok) {
    type_box_->setCurrentIndex(type_model_->getPrimaryIndex());
  }

  // Select align-start.
  alignment_box_->setCurrentIndex(0);

  // Select default fs type.
  const QString default_fs_name = GetSettingsString(kPartitionDefaultFs);
  const FsType default_fs = GetFsTypeByName(default_fs_name);
  const int fs_index = fs_model_->index(default_fs);
  // fs_index might be -1.
  fs_box_->setCurrentIndex(fs_index);

  // Select empty mount-point.
  const int mount_point_index = mount_point_model_->index("");
  mount_point_box_->setCurrentIndex(mount_point_index);

  // Set value range of size_slider_
  size_slider_->setMaximum(partition.getByteLength());
  size_slider_->setMinimum(kMinimumPartitionSize);
}

void NewPartitionFrame::changeEvent(QEvent* event) {
  if (event->type() == QEvent::LanguageChange) {
    title_label_->setText(tr("New Partition"));
    comment_label_->setText(
        tr("Create a new partition and define the type and size"));
    type_label_->setText(tr("Type"));
    alignment_label_->setText(tr("Location"));
    fs_label_->setText(tr("Filesystem"));
    mount_point_label_->setText(tr("Mount point"));
    size_label_->setText(tr("Size"));
    alignment_box_->clear();
    alignment_box_->addItems({tr("Start"), tr("End")});
    cancel_button_->setText(tr("Cancel"));
    create_button_->setText(tr("OK"));
  } else {
    QFrame::changeEvent(event);
  }
}

void NewPartitionFrame::initConnections() {
  connect(fs_box_,
          static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &NewPartitionFrame::onFsChanged);
  connect(mount_point_box_,
          static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
          this, &NewPartitionFrame::onMountPointChanged);

  connect(cancel_button_, &QPushButton::clicked,
          this, &NewPartitionFrame::finished);
  connect(create_button_, &QPushButton::clicked,
          this, &NewPartitionFrame::onCreateButtonClicked);
}

void NewPartitionFrame::initUI() {
  title_label_ = new TitleLabel(tr("New Partition"));
  comment_label_ = new CommentLabel(
      tr("Create a new partition and define the type and size"));
  QHBoxLayout* comment_layout = new QHBoxLayout();
  comment_layout->setContentsMargins(0, 0, 0, 0);
  comment_layout->setSpacing(0);
  comment_layout->addWidget(comment_label_);

  type_label_ = new QLabel(tr("Type"));
  type_label_->setObjectName("type_label");
  type_box_ = new TableComboBox();
  type_model_ = new PartitionTypeModel(type_box_);
  type_box_->setModel(type_model_);

  alignment_label_ = new QLabel(tr("Location"));
  alignment_label_->setObjectName("alignment_label");
  alignment_box_ = new TableComboBox();
  alignment_box_->addItems({tr("Start"), tr("End")});

  fs_label_ = new QLabel(tr("Filesystem"));
  fs_label_->setObjectName("fs_label");
  fs_box_ = new TableComboBox();
  fs_model_ = new FsModel(delegate_, fs_box_);
  fs_box_->setModel(fs_model_);

  mount_point_label_ = new QLabel(tr("Mount point"));
  mount_point_label_->setObjectName("mount_point_label");
  mount_point_box_ = new TableComboBox();
  mount_point_model_ = new MountPointModel(delegate_->allMountPoints(),
                                           mount_point_box_);
  mount_point_box_->setModel(mount_point_model_);

  size_label_ = new QLabel(tr("Size"));
  size_label_->setObjectName("size_label");
  size_slider_ = new PartitionSizeSlider();
  size_slider_->setFixedWidth(mount_point_box_->width());

  QVBoxLayout* content_layout = new QVBoxLayout();
  content_layout->setContentsMargins(0, 0, 0, 0);
  content_layout->setSpacing(3);
  content_layout->addWidget(type_label_);
  content_layout->addWidget(type_box_);
  content_layout->addSpacing(kContentSpacing);
  content_layout->addWidget(alignment_label_);
  content_layout->addWidget(alignment_box_);
  content_layout->addSpacing(kContentSpacing);
  content_layout->addWidget(fs_label_);
  content_layout->addWidget(fs_box_);
  content_layout->addSpacing(kContentSpacing);
  content_layout->addWidget(mount_point_label_);
  content_layout->addWidget(mount_point_box_);
  content_layout->addSpacing(kContentSpacing);
  content_layout->addWidget(size_label_);
  content_layout->addWidget(size_slider_);

  QFrame* content_frame = new QFrame();
  content_frame->setObjectName("content_frame");
  content_frame->setContentsMargins(0, 0, 0, 0);
  content_frame->setLayout(content_layout);
  // Same width as with table combobox.
  content_frame->setFixedWidth(mount_point_box_->width());

  cancel_button_ = new NavButton(tr("Cancel"));
  create_button_ = new NavButton(tr("OK"));

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addStretch();
  layout->addWidget(title_label_, 0, Qt::AlignCenter);
  layout->addSpacing(kMainLayoutSpacing);
  layout->addLayout(comment_layout);
  layout->addStretch();
  layout->addWidget(content_frame, 0, Qt::AlignHCenter);
  layout->addStretch();
  layout->addWidget(cancel_button_, 0, Qt::AlignCenter);
  layout->addSpacing(30);
  layout->addWidget(create_button_, 0, Qt::AlignCenter);

  this->setLayout(layout);
  this->setContentsMargins(0, 0, 0, 0);
  this->setStyleSheet(ReadFile(":/styles/new_partition_frame.css"));
}

void NewPartitionFrame::onCreateButtonClicked() {
  const bool is_primary = type_model_->isPrimary(type_box_->currentIndex());
  const bool is_logical = type_model_->isLogical(type_box_->currentIndex());
  if (!is_primary && !is_logical) {
    // We shall never reach here.
    qCritical() << "Both primary and logical partition are not available!";
    emit this->finished();
    return;
  }
  const PartitionType partition_type = is_primary ? PartitionType::Normal :
                                                    PartitionType::Logical;
  const bool align_start = (alignment_box_->currentIndex() == 0);
  const FsType fs_type = fs_model_->getFs(fs_box_->currentIndex());
  QString mount_point;
  if (mount_point_box_->isVisible()) {
    // Set mount_point only if mount_point_box_ is visible.
    const int index = mount_point_box_->currentIndex();
    mount_point = mount_point_model_->getMountPoint(index);
  }
  // TODO(xushaohua): Calculate exact sectors
  const qint64 total_sectors = size_slider_->value() / partition_.sector_size;
  delegate_->createPartition(partition_, partition_type, align_start, fs_type,
                             mount_point, total_sectors);
  delegate_->refreshVisual();

  emit this->finished();
}

void NewPartitionFrame::onFsChanged(int index) {
  const FsType fs_type = fs_model_->getFs(index);

  // If fs_type is special, no need to display mount-point box.
  const bool visible = IsMountPointSupported(fs_type);
  mount_point_label_->setVisible(visible);
  mount_point_box_->setVisible(visible);

  if (fs_type == FsType::EFI) {
    // Set default size of EFI partition.
    // NOTE(xushaohua): partition size might be less than |default_size|.
    // Its value will also be checked in AdvancedPartitionFrame.
    const qint64 default_size = GetSettingsInt(kPartitionDefaultEFISpace) *
                                kMebiByte;
    size_slider_->setMinimum(default_size);
    size_slider_->setValue(default_size);
  } else {
    // Reset minimum value of size_slider_.
    size_slider_->setMinimum(kMinimumPartitionSize);

    // And set current value to maximum value.
    size_slider_->setValue(size_slider_->maximum());
  }
}

void NewPartitionFrame::onMountPointChanged(int index) {
  const QString mount_point = mount_point_model_->getMountPoint(index);

  if (mount_point == kMountPointBoot) {
    // Set default size for /boot.
    // NOTE(xushaohua): partition size might be less than |default_size|.
    // Its value will also be checked in AdvancedPartitionFrame.
    const qint64 default_size = GetSettingsInt(kPartitionDefaultBootSpace) *
                                kMebiByte;
    size_slider_->setMinimum(default_size);
    size_slider_->setValue(default_size);
  } else {
    // Reset minimum value of size_slider_.
    size_slider_->setMinimum(kMinimumPartitionSize);

    // And set current value to maximum value.
    size_slider_->setValue(size_slider_->maximum());
  }
}

}  // namespace installer