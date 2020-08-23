#include <QDebug>
#include <QFile>

#include "CyACD.h"
namespace {

void appendByte(uint32_t& buffer, const uint8_t byte) {
  buffer <<= 8;
  buffer |= byte;
}

} // namespace

CyACD::CyACD(const QString& filename) {
  QFile f(filename);
  f.open(QIODevice::ReadOnly);
  QTextStream ts(&f);
  appendByte(siliconId, readByte_(ts));
  appendByte(siliconId, readByte_(ts));
  appendByte(siliconId, readByte_(ts));
  appendByte(siliconId, readByte_(ts));

  siliconRevision = readByte_(ts);

  _checksumType = readByte_(ts);
  while (!ts.atEnd()) {
    readRow_(ts);
  }
  f.close();
}

CyACD::~CyACD() { data.clear(); }

uint8_t CyACD::readByte_(QTextStream &ts) {
  QString buf = ts.read(2);
  if (buf.length() != 2) {
    throw("File corrupted - incomplete hex-encoded byte!");
  }
  bool ok;
  uint8_t retval = buf.toUInt(&ok, 16);
  if (!ok) {
    throw("File corrupted - not a hex string!");
  }
  runningSum_ += retval;
  return retval;
}

uint16_t CyACD::readUInt16_(QTextStream &ts) {
  return (readByte_(ts) << 8) + readByte_(ts);
}

void CyACD::readRow_(QTextStream &ts) {
  char rs;
  ts >> rs;
  if (rs == ':') {
    runningSum_ = 0;
    auto row = std::make_unique<CyACD_row>();
    row->array = readByte_(ts);
    row->row = readUInt16_(ts);
    uint16_t dataLength = readUInt16_(ts);
    for (uint16_t i = 0; i < dataLength; i++) {
      row->data.append(readByte_(ts));
    }
    // Must calculate checksum here - reading bytes changes running sum
    uint8_t checksum = (1 + ~runningSum_) & 0xff;
    row->checksum = readByte_(ts);
    if (checksum != row->checksum) {
      throw("File corrupted - row checksum mismatch!");
    }
    data.insert(data.begin(), std::move(row));
  }
}
