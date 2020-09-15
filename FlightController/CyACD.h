#pragma once

#include <QString>
#include <QTextStream>
#include <memory>

using ChecksumBufferType = uint16_t;

struct CyACD_row {
  uint8_t array;
  uint16_t row;
  ChecksumBufferType checksum;
  QByteArray data;
};

class CyACD {
 public:
  CyACD(const QString& filename);
  ~CyACD();

  bool loaded{false};
  uint32_t siliconId;
  uint8_t siliconRevision;
  std::vector<std::unique_ptr<CyACD_row>> data;

 private:
  uint8_t readByte_(QTextStream &ts);
  uint16_t readUInt16_(QTextStream &ts);
  void readRow_(QTextStream &ts);
  void initChecksum_();
  void calculateChecksum_();

  uint8_t checksumType_; // currently unused
  ChecksumBufferType runningSum_{0};

};
