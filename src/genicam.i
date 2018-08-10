%module genicam
%{
#include <system.h>
#include <interface.h>
#include <device.h>
#include <stream.h>
#include <buffer.h>
#include <config.h>
#include "../rc_genicam_api/genicam/library/CPP/include/GenTL/PFNC.h"
using namespace rcg;
%}

%include <std_shared_ptr.i>
%include <std_vector.i>
%include <std_string.i>

%shared_ptr(GenApi::CNodeMapRef)
%shared_ptr(rcg::System)
%shared_ptr(rcg::Interface)
%shared_ptr(rcg::Device)
%shared_ptr(rcg::GenTLWrapper)
%shared_ptr(rcg::Stream)
%shared_ptr(rcg::Buffer)

%template(GSystemVector) std::vector<std::shared_ptr<rcg::System> >;
%template(GInterfaceVector) std::vector<std::shared_ptr<rcg::Interface> >;
%template(GDeviceVector) std::vector<std::shared_ptr<rcg::Device> >;
%template(GStreamVector) std::vector<std::shared_ptr<rcg::Stream> >;
%template(GBufferVector) std::vector<std::shared_ptr<rcg::Buffer> >;

%catches(std::exception, std::string);

%include "../rc_genicam_api/rc_genicam_api/system.h"
%include "../rc_genicam_api/rc_genicam_api/interface.h"
%include "../rc_genicam_api/rc_genicam_api/device.h"
%include "../rc_genicam_api/rc_genicam_api/stream.h"
%include "../rc_genicam_api/rc_genicam_api/buffer.h"
%include "../rc_genicam_api/rc_genicam_api/config.h"
%include "../rc_genicam_api/genicam/library/CPP/include/GenTL/PFNC.h"
