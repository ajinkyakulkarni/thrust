// Copyright (c) 2013 Stanislas Polu.
// Copyright (c) 2012 The Chromium Authors.
// See the LICENSE file.

#include "breach/app/breach_main_delegate.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/content_switches.h"
#include "content/public/common/url_constants.h"
/* TODO(spolu): TO REMOVE? */
//#include "net/cookies/cookie_monster.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"
#include "ui/base/ui_base_switches.h"

#include "third_party/node/src/node_version.h"

#include "breach/common/breach_switches.h"
#include "breach/browser/breach_content_browser_client.h"
/* TODO(spolu): NW */
//#include "content/nw/src/nw_version.h"
#include "breach/renderer/shell_content_renderer_client.h"
#include "breach/shell_browser_main.h"

#include <stdio.h>

#include "ipc/ipc_message.h"  // For IPC_MESSAGE_LOG_ENABLED.

#if defined(IPC_MESSAGE_LOG_ENABLED)
#define IPC_MESSAGE_MACROS_LOG_ENABLED
#include "content/public/common/content_ipc_logging.h"
#define IPC_LOG_TABLE_ADD_ENTRY(msg_id, logger) \
    content::RegisterIPCLogger(msg_id, logger)
/* TODO(spolu): NW */
//#include "content/nw/src/common/common_message_generator.h"
//#include "components/autofill/common/autofill_messages.h"
#endif

#if defined(OS_MACOSX)
#include "content/shell/paths_mac.h"
#endif  // OS_MACOSX

#if defined(OS_WIN)
#include <initguid.h>
#include "base/logging_win.h"
#endif  // OS_WIN

using namespace content;

namespace {

#if defined(OS_WIN)
/* TODO(spolu): Create Key for Breach */
// If "Breach" doesn't show up in your list of trace providers in
// Sawbuck, add these registry entries to your machine (NOTE the optional
// Wow6432Node key for x64 machines):
// 1. Find:  HKLM\SOFTWARE\[Wow6432Node\]Google\Sawbuck\Providers
// 2. Add a subkey with the name "{6A3E50A4-7E15-4099-8413-EC94D8C2A4B6}"
// 3. Add these values:
//    "default_flags"=dword:00000001
//    "default_level"=dword:00000004
//    @="Breach"

// {6A3E50A4-7E15-4099-8413-EC94D8C2A4B6}
const GUID kBreachProviderName = {
    0x6a3e50a4, 0x7e15, 0x4099,
        { 0x84, 0x13, 0xec, 0x94, 0xd8, 0xc2, 0xa4, 0xb6 } };
#endif

void InitLogging() {
  base::FilePath log_filename;
  PathService::Get(base::DIR_EXE, &log_filename);
  log_filename = log_filename.AppendASCII("breach.log");
  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_ALL;
  settings.log_file = log_filename.value().c_str();
  settings.delete_old = logging::DELETE_OLD_LOG_FILE;
  logging::InitLogging(settings);
  logging::SetLogItems(true, true, true, true);
}

}  // namespace

namespace breach {

BreachMainDelegate::BreachMainDelegate() {
}

BreachMainDelegate::~BreachMainDelegate() {
}

bool BreachMainDelegate::BasicStartupComplete(int* exit_code) {
#if defined(OS_WIN)
  // Enable trace control and transport through event tracing for Windows.
  logging::LogEventProvider::Initialize(kBreachProviderName);
#endif

#if defined(OS_MACOSX)
  // Needs to happen before InitializeResourceBundle() and before
  // WebKitTestPlatformInitialize() are called.
  OverrideFrameworkBundlePath();
  OverrideChildProcessPath();
#endif  // OS_MACOSX

  InitLogging();
  /* TODO(spolu): TO REMOVE? */
  net::CookieMonster::EnableFileScheme();

  SetContentClient(&breach_content_client_);
  return false;
}

void BreachMainDelegate::PreSandboxStartup() {
  InitializeResourceBundle();
}

void BreachMainDelegate::InitializeResourceBundle() {
  base::FilePath pak_file;
#if defined(OS_MACOSX)
  pak_file = GetResourcesPakFilePath();
#else
  base::FilePath pak_dir;
  PathService::Get(base::DIR_MODULE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("breach.pak"));
#endif
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
}

ContentBrowserClient* BreachMainDelegate::CreateContentBrowserClient() {
  /* TODO(spolu): renaming post file creation */
  browser_client_.reset(new ShellContentBrowserClient);
  return browser_client_.get();
}

ContentRendererClient* BreachMainDelegate::CreateContentRendererClient() {
  /* TODO(spolu): renaming post file creation */
  renderer_client_.reset(new ShellContentRendererClient);
  return renderer_client_.get();
}

}  // namespace breach
