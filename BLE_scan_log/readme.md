# バージョン
* V1.0.0: 初回作成バージョン
# 概要
アドバタイズされたBLEデータを受信し、スプレッドシートにログ出力する。
ログモードとして 'add', 'update' がある。
'add' は add シートに採取ログを先頭に追加して行く。
'update' は update シートに最初のログを2行目に登録後、それ以降のログは常に3行目を更新する。
# ログ項目
受信日時	送信日時	気圧	温度	湿度	MAC
# 出力先
BLE_scan_log.ino の host でスプレッドシートのデプロイ URL を設定。
# ログ対象
マニュファクチャコード(TARGET_MANU を define 定義)またはセンサーMACアドレス(TARGET_ADDR を define 定義)を指定する。
* マニュファクチャコード
  TARGET_MANU_STR, TARGET_MANU_HEX にマニュファクチャコードを設定。
  CHECK_ADDR に MACアドレスを指定した場合 MACアドレスもチェックする。
* MAC アドレス指定
  TARGET_ADDR に センサーのMACアドレスを指定。
# ログモード
* sendSensorData.ino の json_request["mode"] で "add" 又は "update" を指定する。
