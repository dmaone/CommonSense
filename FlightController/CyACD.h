#pragma once

#include <QString>
#include <QTextStream>

typedef struct {
  uint8_t array;
  uint16_t row;
  uint16_t checksum;
  QByteArray data;

} CyACD_row;

class CyACD {
 public:
  CyACD(const QString& filename);
  ~CyACD();
  bool loaded;
  uint32_t siliconId;
  uint8_t siliconRevision;
  std::vector<CyACD_row *> data;

 private:
  uint8_t _checksumType;
  uint8_t _readByte(QTextStream &ts);
  uint16_t _readUInt16(QTextStream &ts);
  uint16_t checksum{0};
  uint16_t _checksum{0};
  void _readRow(QTextStream &ts);
  void _resetChecksum();
  void _calculateChecksum();
  void _verifyChecksum(CyACD_row *row);
};
