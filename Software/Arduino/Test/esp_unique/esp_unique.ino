unsigned long macHash(const unsigned char* mac)
{
    unsigned long hash = 5381;
    int c;

    for (int i = 0; i < 6; i++) {
        c = mac[i];
        hash = ((hash << 5) + hash) + c; // djb2 hash algorithm
    }

    return hash;
}

unsigned long getUnique(){
    uint8_t factory_mac[6];

    esp_err_t ret = esp_efuse_mac_get_default(factory_mac);
    if (ret == ESP_OK) {
        return macHash(factory_mac);;
    } else {
        return 0;
    }
}

void setup() {
  Serial.begin(115200);

}

void loop() {
  Serial.printf("ESP Unique: %lu\n", getUnique());
  delay(1000);

}
