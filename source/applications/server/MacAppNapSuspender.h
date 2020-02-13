class AppNapSuspenderPrivate;
class AppNapSuspender {
public:
AppNapSuspender();
~AppNapSuspender();

void suspend();
void resume();
private:
AppNapSuspenderPrivate *p;
};
