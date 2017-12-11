#include <QDebug>
#include <QFile>

#include "CyACD.h"

CyACD::CyACD(QString filename) {
  loaded = false;
  QFile f(filename);
  f.open(QIODevice::ReadOnly);
  QTextStream ts(&f);
  siliconId = _readByte(ts) << 24;
  siliconId |= _readByte(ts) << 16;
  siliconId |= _readByte(ts) << 8;
  siliconId |= _readByte(ts);
  siliconRevision = _readByte(ts);
  _checksumType = _readByte(ts);
  while (!ts.atEnd()) {
    _readRow(ts);
  }
  f.close();
}

CyACD::~CyACD(void) { data.clear(); }

uint8_t CyACD::_readByte(QTextStream &ts) {
  QString buf = ts.read(2);
  if (buf.length() != 2)
    throw("File corrupted - expecting more bytes!");
  bool ok;
  uint8_t retval = buf.toUInt(&ok, 16);
  if (!ok)
    throw("File corrupted - not a hex string!");
  checksum += retval;
  return retval;
}

uint16_t CyACD::_readUInt16(QTextStream &ts) {
  return (_readByte(ts) << 8) + _readByte(ts);
}

void CyACD::_readRow(QTextStream &ts) {
  char rs;
  ts >> rs;
  if (rs == ':') {
    _resetChecksum();
    CyACD_row *row = new CyACD_row();
    row->array = _readByte(ts);
    row->row = _readUInt16(ts);
    uint16_t dataLength = _readUInt16(ts);
    for (uint16_t i = 0; i < dataLength; i++) {
      row->data.append(_readByte(ts));
    }
    _calculateChecksum();
    row->checksum = _readByte(ts);
    _verifyChecksum(row);
    // std::vector<CyACD_row *>::iterator it = data.begin();
    data.insert(data.begin(), row);
  }
}

void CyACD::_resetChecksum(void) {
  checksum = 0;
  _checksum = 0;
}

void CyACD::_calculateChecksum(void) {
  _checksum = (1 + ~checksum) & 0xff; // 2's complement
}

void CyACD::_verifyChecksum(CyACD_row *row) {
  if (_checksum != row->checksum)
    throw("File corrupted - row checksum mismatch!");
}
