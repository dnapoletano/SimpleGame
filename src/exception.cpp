#include <cxxabi.h>
#include <format>
#include <dlfcn.h>

#include "exception.hpp"

namespace game {
   StackTrace::StackTrace(const int skip_lines) {
      void *trace[MAX_BACKTRACE_DEPTH];
      const int depth = backtrace(trace, MAX_BACKTRACE_DEPTH);
      for (int n = 0; n <depth; ++n) {
         const auto addr = reinterpret_cast<size_t>(trace[n]);
         Dl_info info;
         if (dladdr(trace[n],&info) && info.dli_fname && info.dli_fname[0]) {
            const auto symaddr = reinterpret_cast<size_t>(info.dli_saddr);
            if (symaddr==reinterpret_cast<size_t>(nullptr)) continue;
            auto symname = (not info.dli_sname or not info.dli_sname[0])
               ? "<unknown function>" : info.dli_sname;
            std::string linfo;
            const auto lib_addr = reinterpret_cast<size_t>(info.dli_fbase);
            const auto offset = (addr >= lib_addr) ? addr - lib_addr : lib_addr - addr;
            char cmd[4096];
            std::snprintf(cmd,4096,"addr2line -se %s 0x%016lx 2>/dev/null",info.dli_fname,offset);
            if (FILE *pf = popen(cmd,"r")) {
               char buf[2048];
               if (fgets(buf,2048,pf)) {
                  linfo = buf;
                  linfo.pop_back();
               }
               if (linfo=="??:0") {
                  pclose(pf);
                  snprintf(cmd,4096,"addr2line -se %s 0x%016lx 2>/dev/null",
                      info.dli_fname,addr);
                  pf=popen(cmd,"r");
                  if (fgets(buf,2048,pf)) {
                     linfo=buf;
                     linfo.pop_back();
                  }
                  if (linfo=="??:0") linfo="";
               }
               pclose(pf);
            }
            if (n < skip_lines) continue;
            _trace += std::format(
               "{0:#06x} -> {1} ({2}) from {3}\n",
               addr,_demangle(symname),linfo,info.dli_fname
            );
            if (std::string(info.dli_sname)=="main") break;
         }
      }
   }

   auto StackTrace::_demangle(const std::string& name) -> std::string {
      int s;
      size_t len(name.length());
      const auto res = abi::__cxa_demangle(name.c_str(),nullptr,&len,&s);
      return s == 0? std::string(res):name;
   }


   Exception::Exception(const std::string& what)
      : std::runtime_error(what),
        _trace(5)
   {

   }

   auto Exception::stackTrace() const -> std::string {
      return _trace.getTrace();
   }

}
