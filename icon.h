//const uint_8 data[] =
//{
//0x06, 0x60, 0x06, 0xe3, 0x0c, 0xe3, 0x00, 0x01, 0x02, 0x05, 0x06, 0x6d, 0x4e, 0x6d, 0x5e, 0x6d, 0x7e, 0x6d, 0x36, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x07, 0xfd, 0x03, 0x98, 0x00, 0x00
//};



void logo1 (uint8_t x){
  static const uint8_t logo1[] PROGMEM = {
    16,16,
    0x06, 0x60, 0x06, 0xe3, 0x0c, 0xe3, 0x00, 0x01, 0x02, 0x05, 0x06, 0x6d, 0x4e, 0x6d, 0x5e, 0x6d, 0x7e, 0x6d, 0x36, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x07, 0xfd, 0x03, 0x98, 0x00, 0x00
  };
  Disp.drawBitmap(x,0,logo1);
}
