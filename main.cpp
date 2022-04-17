#include <iostream>
#include <atomic>
#include <unistd.h>
#include <signal.h>

// 終了処理へ入るフラグ
std::atomic<bool> stop(false);

// SIGINT捕獲時にフラグをセットする
void signal_handler(int sig, siginfo_t *info, void *ctx) {
  stop.store(true);
}

// 実験用のクラス
// デストラクタの呼び出されるとメッセージを出力する
class Test {
  public:
    Test() {};
    ~Test() {
      std::cout << "Called destructor. Bye!" << std::endl;
    };
    void print() {
      std::cout << "Hello" << std::endl;
    };
};

int main(int argc, char *argv[]) {
  // SIGINTの捕獲設定
  struct sigaction sa_sigabrt;
  memset(&sa_sigabrt, 0, sizeof(sa_sigabrt));
  sa_sigabrt.sa_sigaction = signal_handler;
  sa_sigabrt.sa_flags = SA_SIGINFO;
  if (sigaction(SIGINT, &sa_sigabrt, NULL) < 0) {
    exit(1);
  }

  // 実験開始
  Test ts;
  for (;;) {
    ts.print();
    usleep(500000);
    if (stop.load() == true) {
      std::cout << "\tStop action\n";
      break;
    }
  }

  return 0;
}
