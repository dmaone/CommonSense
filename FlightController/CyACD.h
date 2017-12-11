#ifndef CYACD_H
#define CYACD_H

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
  CyACD(QString filename);
  ~CyACD();
  bool loaded;
  uint32_t siliconId;
  uint8_t siliconRevision;
  std::vector<CyACD_row *> data;

private:
  uint8_t _checksumType;
  uint8_t _readByte(QTextStream &ts);
  uint16_t _readUInt16(QTextStream &ts);
  uint16_t checksum;
  uint16_t _checksum;
  void _readRow(QTextStream &ts);
  void _resetChecksum(void);
  void _calculateChecksum(void);
  void _verifyChecksum(CyACD_row *row);
};

#endif // CYACD_H
