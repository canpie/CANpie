#include "MacAppNapSuspender.h"

#import <Foundation/NSString.h>
#include <Foundation/NSProcessInfo.h>

class AppNapSuspenderPrivate
{
public:
   id<NSObject> activityId;
};

AppNapSuspender::AppNapSuspender() :
 p(new AppNapSuspenderPrivate)
{}

AppNapSuspender::~AppNapSuspender()
{
delete p;
}

void AppNapSuspender::suspend()
{
   p->activityId = [[NSProcessInfo processInfo ] beginActivityWithOptions: NSActivityUserInitiated reason:@"Good reason"];
}

void AppNapSuspender::resume()
{
   [[NSProcessInfo processInfo ] endActivity:p->activityId];
}
