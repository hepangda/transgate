//  Copyright 2018 hepangda
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "static_provider.h"

#include <cctype>
#include <iostream>
#include "../../utils/file_reader.h"

namespace tg {

namespace detail {
const char *mimeTable(const char *ext) {
#define ie(str, mime) if (strcasecmp(ext, str) == 0) return mime
  ie (".tif", "image/tiff");
  ie (".001", "application/x-001");
  ie (".301", "application/x-301");
  ie (".323", "text/h323");
  ie (".906", "application/x-906");
  ie (".907", "drawing/907");
  ie (".a11", "application/x-a11");
  ie (".acp", "audio/x-mei-aac");
  ie (".ai", "application/postscript");
  ie (".aif", "audio/aiff");
  ie (".aifc", "audio/aiff");
  ie (".aiff", "audio/aiff");
  ie (".anv", "application/x-anv");
  ie (".asa", "text/asa");
  ie (".asf", "video/x-ms-asf");
  ie (".asp", "text/asp");
  ie (".asx", "video/x-ms-asf");
  ie (".au", "audio/basic");
  ie (".avi", "video/avi");
  ie (".awf", "application/vnd.adobe.workflow");
  ie (".biz", "text/xml");
  ie (".bmp", "application/x-bmp");
  ie (".bot", "application/x-bot");
  ie (".c4t", "application/x-c4t");
  ie (".c90", "application/x-c90");
  ie (".cal", "application/x-cals");
  ie (".cat", "application/vnd.ms-pki.seccat");
  ie (".cdf", "application/x-netcdf");
  ie (".cdr", "application/x-cdr");
  ie (".cel", "application/x-cel");
  ie (".cer", "application/x-x509-ca-cert");
  ie (".cg4", "application/x-g4");
  ie (".cgm", "application/x-cgm");
  ie (".cit", "application/x-cit");
  ie (".class", "java/*");
  ie (".cml", "text/xml");
  ie (".cmp", "application/x-cmp");
  ie (".cmx", "application/x-cmx");
  ie (".cot", "application/x-cot");
  ie (".crl", "application/pkix-crl");
  ie (".crt", "application/x-x509-ca-cert");
  ie (".csi", "application/x-csi");
  ie (".css", "text/css");
  ie (".cut", "application/x-cut");
  ie (".dbf", "application/x-dbf");
  ie (".dbm", "application/x-dbm");
  ie (".dbx", "application/x-dbx");
  ie (".dcd", "text/xml");
  ie (".dcx", "application/x-dcx");
  ie (".der", "application/x-x509-ca-cert");
  ie (".dgn", "application/x-dgn");
  ie (".dib", "application/x-dib");
  ie (".dll", "application/x-msdownload");
  ie (".doc", "application/msword");
  ie (".dot", "application/msword");
  ie (".drw", "application/x-drw");
  ie (".dtd", "text/xml");
  ie (".dwf", "Model/vnd.dwf");
  ie (".dwf", "application/x-dwf");
  ie (".dwg", "application/x-dwg");
  ie (".dxb", "application/x-dxb");
  ie (".dxf", "application/x-dxf");
  ie (".edn", "application/vnd.adobe.edn");
  ie (".emf", "application/x-emf");
  ie (".eml", "message/rfc822");
  ie (".ent", "text/xml");
  ie (".epi", "application/x-epi");
  ie (".eps", "application/x-ps");
  ie (".eps", "application/postscript");
  ie (".etd", "application/x-ebx");
  ie (".exe", "application/x-msdownload");
  ie (".fax", "image/fax");
  ie (".fdf", "application/vnd.fdf");
  ie (".fif", "application/fractals");
  ie (".fo", "text/xml");
  ie (".frm", "application/x-frm");
  ie (".g4", "application/x-g4");
  ie (".gbr", "application/x-gbr");
  ie (".", "application/x-");
  ie (".gif", "image/gif");
  ie (".gl2", "application/x-gl2");
  ie (".gp4", "application/x-gp4");
  ie (".hgl", "application/x-hgl");
  ie (".hmr", "application/x-hmr");
  ie (".hpg", "application/x-hpgl");
  ie (".hpl", "application/x-hpl");
  ie (".hqx", "application/mac-binhex40");
  ie (".hrf", "application/x-hrf");
  ie (".hta", "application/hta");
  ie (".htc", "text/x-component");
  ie (".htm", "text/html");
  ie (".html", "text/html");
  ie (".htt", "text/webviewhtml");
  ie (".htx", "text/html");
  ie (".icb", "application/x-icb");
  ie (".ico", "image/x-icon");
  ie (".ico", "application/x-ico");
  ie (".iff", "application/x-iff");
  ie (".ig4", "application/x-g4");
  ie (".igs", "application/x-igs");
  ie (".iii", "application/x-iphone");
  ie (".img", "application/x-img");
  ie (".ins", "application/x-internet-signup");
  ie (".isp", "application/x-internet-signup");
  ie (".IVF", "video/x-ivf");
  ie (".java", "java/*");
  ie (".jfif", "image/jpeg");
  ie (".jpe", "image/jpeg");
  ie (".jpe", "application/x-jpe");
  ie (".jpeg", "image/jpeg");
  ie (".jpg", "image/jpeg");
  ie (".jpg", "application/x-jpg");
  ie (".js", "application/x-javascript");
  ie (".jsp", "text/html");
  ie (".la1", "audio/x-liquid-file");
  ie (".lar", "application/x-laplayer-reg");
  ie (".latex", "application/x-latex");
  ie (".lavs", "audio/x-liquid-secure");
  ie (".lbm", "application/x-lbm");
  ie (".lmsff", "audio/x-la-lms");
  ie (".ls", "application/x-javascript");
  ie (".ltr", "application/x-ltr");
  ie (".m1v", "video/x-mpeg");
  ie (".m2v", "video/x-mpeg");
  ie (".m3u", "audio/mpegurl");
  ie (".m4e", "video/mpeg4");
  ie (".mac", "application/x-mac");
  ie (".man", "application/x-troff-man");
  ie (".math", "text/xml");
  ie (".mdb", "application/msaccess");
  ie (".mdb", "application/x-mdb");
  ie (".mfp", "application/x-shockwave-flash");
  ie (".mht", "message/rfc822");
  ie (".mhtml", "message/rfc822");
  ie (".mi", "application/x-mi");
  ie (".mid", "audio/mid");
  ie (".midi", "audio/mid");
  ie (".mil", "application/x-mil");
  ie (".mml", "text/xml");
  ie (".mnd", "audio/x-musicnet-download");
  ie (".mns", "audio/x-musicnet-stream");
  ie (".mocha", "application/x-javascript");
  ie (".movie", "video/x-sgi-movie");
  ie (".mp1", "audio/mp1");
  ie (".mp2", "audio/mp2");
  ie (".mp2v", "video/mpeg");
  ie (".mp3", "audio/mp3");
  ie (".mp4", "video/mpeg4");
  ie (".mpa", "video/x-mpg");
  ie (".mpd", "application/vnd.ms-project");
  ie (".mpe", "video/x-mpeg");
  ie (".mpeg", "video/mpg");
  ie (".mpg", "video/mpg");
  ie (".mpga", "audio/rn-mpeg");
  ie (".mpp", "application/vnd.ms-project");
  ie (".mps", "video/x-mpeg");
  ie (".mpt", "application/vnd.ms-project");
  ie (".mpv", "video/mpg");
  ie (".mpv2", "video/mpeg");
  ie (".mpw", "application/vnd.ms-project");
  ie (".mpx", "application/vnd.ms-project");
  ie (".mtx", "text/xml");
  ie (".mxp", "application/x-mmxp");
  ie (".net", "image/pnetvue");
  ie (".nrf", "application/x-nrf");
  ie (".nws", "message/rfc822");
  ie (".odc", "text/x-ms-odc");
  ie (".out", "application/x-out");
  ie (".p10", "application/pkcs10");
  ie (".p12", "application/x-pkcs12");
  ie (".p7b", "application/x-pkcs7-certificates");
  ie (".p7c", "application/pkcs7-mime");
  ie (".p7m", "application/pkcs7-mime");
  ie (".p7r", "application/x-pkcs7-certreqresp");
  ie (".p7s", "application/pkcs7-signature");
  ie (".pc5", "application/x-pc5");
  ie (".pci", "application/x-pci");
  ie (".pcl", "application/x-pcl");
  ie (".pcx", "application/x-pcx");
  ie (".pdf", "application/pdf");
  ie (".pdf", "application/pdf");
  ie (".pdx", "application/vnd.adobe.pdx");
  ie (".pfx", "application/x-pkcs12");
  ie (".pgl", "application/x-pgl");
  ie (".pic", "application/x-pic");
  ie (".pko", "application/vnd.ms-pki.pko");
  ie (".pl", "application/x-perl");
  ie (".plg", "text/html");
  ie (".pls", "audio/scpls");
  ie (".plt", "application/x-plt");
  ie (".png", "image/png");
  ie (".png", "application/x-png");
  ie (".pot", "application/vnd.ms-powerpoint");
  ie (".ppa", "application/vnd.ms-powerpoint");
  ie (".ppm", "application/x-ppm");
  ie (".pps", "application/vnd.ms-powerpoint");
  ie (".ppt", "application/vnd.ms-powerpoint");
  ie (".ppt", "application/x-ppt");
  ie (".pr", "application/x-pr");
  ie (".prf", "application/pics-rules");
  ie (".prn", "application/x-prn");
  ie (".prt", "application/x-prt");
  ie (".ps", "application/x-ps");
  ie (".ps", "application/postscript");
  ie (".ptn", "application/x-ptn");
  ie (".pwz", "application/vnd.ms-powerpoint");
  ie (".r3t", "text/vnd.rn-realtext3d");
  ie (".ra", "audio/vnd.rn-realaudio");
  ie (".ram", "audio/x-pn-realaudio");
  ie (".ras", "application/x-ras");
  ie (".rat", "application/rat-file");
  ie (".rdf", "text/xml");
  ie (".rec", "application/vnd.rn-recording");
  ie (".red", "application/x-red");
  ie (".rgb", "application/x-rgb");
  ie (".rjs", "application/vnd.rn-realsystem-rjs");
  ie (".rjt", "application/vnd.rn-realsystem-rjt");
  ie (".rlc", "application/x-rlc");
  ie (".rle", "application/x-rle");
  ie (".rm", "application/vnd.rn-realmedia");
  ie (".rmf", "application/vnd.adobe.rmf");
  ie (".rmi", "audio/mid");
  ie (".rmj", "application/vnd.rn-realsystem-rmj");
  ie (".rmm", "audio/x-pn-realaudio");
  ie (".rmp", "application/vnd.rn-rn_music_package");
  ie (".rms", "application/vnd.rn-realmedia-secure");
  ie (".rmvb", "application/vnd.rn-realmedia-vbr");
  ie (".rmx", "application/vnd.rn-realsystem-rmx");
  ie (".rnx", "application/vnd.rn-realplayer");
  ie (".rp", "image/vnd.rn-realpix");
  ie (".rpm", "audio/x-pn-realaudio-plugin");
  ie (".rsml", "application/vnd.rn-rsml");
  ie (".rt", "text/vnd.rn-realtext");
  ie (".rtf", "application/msword");
  ie (".rtf", "application/x-rtf");
  ie (".rv", "video/vnd.rn-realvideo");
  ie (".sam", "application/x-sam");
  ie (".sat", "application/x-sat");
  ie (".sdp", "application/sdp");
  ie (".sdw", "application/x-sdw");
  ie (".sit", "application/x-stuffit");
  ie (".slb", "application/x-slb");
  ie (".sld", "application/x-sld");
  ie (".slk", "drawing/x-slk");
  ie (".smi", "application/smil");
  ie (".smil", "application/smil");
  ie (".smk", "application/x-smk");
  ie (".snd", "audio/basic");
  ie (".sol", "text/plain");
  ie (".sor", "text/plain");
  ie (".spc", "application/x-pkcs7-certificates");
  ie (".spl", "application/futuresplash");
  ie (".spp", "text/xml");
  ie (".ssm", "application/streamingmedia");
  ie (".sst", "application/vnd.ms-pki.certstore");
  ie (".stl", "application/vnd.ms-pki.stl");
  ie (".stm", "text/html");
  ie (".sty", "application/x-sty");
  ie (".svg", "text/xml");
  ie (".swf", "application/x-shockwave-flash");
  ie (".tdf", "application/x-tdf");
  ie (".tg4", "application/x-tg4");
  ie (".tga", "application/x-tga");
  ie (".tif", "image/tiff");
  ie (".tif", "application/x-tif");
  ie (".tiff", "image/tiff");
  ie (".tld", "text/xml");
  ie (".top", "drawing/x-top");
  ie (".torrent", "application/x-bittorrent");
  ie (".tsd", "text/xml");
  ie (".txt", "text/plain");
  ie (".uin", "application/x-icq");
  ie (".uls", "text/iuls");
  ie (".vcf", "text/x-vcard");
  ie (".vda", "application/x-vda");
  ie (".vdx", "application/vnd.visio");
  ie (".vml", "text/xml");
  ie (".vpg", "application/x-vpeg005");
  ie (".vsd", "application/vnd.visio");
  ie (".vsd", "application/x-vsd");
  ie (".vss", "application/vnd.visio");
  ie (".vst", "application/vnd.visio");
  ie (".vst", "application/x-vst");
  ie (".vsw", "application/vnd.visio");
  ie (".vsx", "application/vnd.visio");
  ie (".vtx", "application/vnd.visio");
  ie (".vxml", "text/xml");
  ie (".wav", "audio/wav");
  ie (".wax", "audio/x-ms-wax");
  ie (".wb1", "application/x-wb1");
  ie (".wb2", "application/x-wb2");
  ie (".wb3", "application/x-wb3");
  ie (".wbmp", "image/vnd.wap.wbmp");
  ie (".wiz", "application/msword");
  ie (".wk3", "application/x-wk3");
  ie (".wk4", "application/x-wk4");
  ie (".wkq", "application/x-wkq");
  ie (".wks", "application/x-wks");
  ie (".wm", "video/x-ms-wm");
  ie (".wma", "audio/x-ms-wma");
  ie (".wmd", "application/x-ms-wmd");
  ie (".wmf", "application/x-wmf");
  ie (".wml", "text/vnd.wap.wml");
  ie (".wmv", "video/x-ms-wmv");
  ie (".wmx", "video/x-ms-wmx");
  ie (".wmz", "application/x-ms-wmz");
  ie (".wp6", "application/x-wp6");
  ie (".wpd", "application/x-wpd");
  ie (".wpg", "application/x-wpg");
  ie (".wpl", "application/vnd.ms-wpl");
  ie (".wq1", "application/x-wq1");
  ie (".wr1", "application/x-wr1");
  ie (".wri", "application/x-wri");
  ie (".wrk", "application/x-wrk");
  ie (".ws", "application/x-ws");
  ie (".ws2", "application/x-ws");
  ie (".wsc", "text/scriptlet");
  ie (".wsdl", "text/xml");
  ie (".wvx", "video/x-ms-wvx");
  ie (".xdp", "application/vnd.adobe.xdp");
  ie (".xdr", "text/xml");
  ie (".xfd", "application/vnd.adobe.xfd");
  ie (".xfdf", "application/vnd.adobe.xfdf");
  ie (".xhtml", "text/html");
  ie (".xls", "application/vnd.ms-excel");
  ie (".xls", "application/x-xls");
  ie (".xlw", "application/x-xlw");
  ie (".xml", "text/xml");
  ie (".xpl", "audio/scpls");
  ie (".xq", "text/xml");
  ie (".xql", "text/xml");
  ie (".xquery", "text/xml");
  ie (".xsd", "text/xml");
  ie (".xsl", "text/xml");
  ie (".xslt", "text/xml");
  ie (".xwd", "application/x-xwd");
  ie (".x_b", "application/x-x_b");
  ie (".sis", "application/vnd.symbian.install");
  ie (".sisx", "application/vnd.symbian.install");
  ie (".x_t", "application/x-x_t");
  ie (".ipa", "application/vnd.iphone");
  ie (".apk", "application/vnd.android.package-archive");
  ie (".xap", "application/x-silverlight-app");
#undef ie
  return "application/octet-stream";
}
bool isFilename(char x) {
  return !(x == '?' || x == '\\' || x == '/' || x == '*' ||
      x == '\"' || x == '\'' || x == '<' || x == '>' || x == '|');
}
}

void StaticProvider::provide() {
  if (!request_->good()) {
//    return;
  }
  request_->set_code(kHCOk);

  regularProvide(true, 200);
  writeCRLF();
  // std::make_shared<FileReader>(wwwrootfd, request_.uri())
  write_loop_->appendSend();
  write_loop_->appendSendfile(std::make_shared<FileReader>("/home/pangda/wwwroot/index.html"));
}

void StaticProvider::provideError() {
  regularProvide(request_->isKeepalive(), 22, "text/html");
  writeCRLF();

}

void StaticProvider::regularProvide(bool keep_alive, int content_length, const char *mime) {
  writeHead(request_->major_version(), request_->minor_version(), request_->code());
  writeItemDate();
  writeItemConnection(keep_alive);
  writeItem("Content-Type", mime);
  writeItem("Content-Length", std::to_string(content_length).c_str());
}

void StaticProvider::regularProvide(bool keep_alive, int content_length) {
  std::cout << "#" << adaptMIME() << "#\n";
  regularProvide(keep_alive, content_length, adaptMIME());
}

const char *StaticProvider::adaptMIME() {
  const char *ext_start = request_->uri().readptr() + request_->uri().size(),
      *ext_end = ext_start;

  for (; ext_start != request_->uri().readptr(); ext_start--) {
    if (*ext_start == '.') {
      break;
    } else if (!detail::isFilename(*ext_start)) {
      ext_end = ext_start;
    }
  }
  std::cout << "$" << std::string(ext_start,
                                  static_cast<unsigned long>(std::distance(ext_start, ext_end))) << "$\n";
  if (ext_start == request_->uri().readptr()) {
    return "text/html";
  } else {

    return detail::mimeTable(std::string(ext_start,
                                         static_cast<unsigned long>(std::distance(ext_start, ext_end))).c_str());
  }
}

}