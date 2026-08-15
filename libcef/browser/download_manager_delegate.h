// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_DOWNLOAD_MANAGER_DELEGATE_H_
#define CEF_LIBCEF_BROWSER_DOWNLOAD_MANAGER_DELEGATE_H_
#pragma once

#include <set>

#include "cef/libcef/features/features.h"
#if BUILDFLAG(ENABLE_CEF_DOWNLOADS)
#include "libcef/browser/browser_host_base.h"
#endif

#include "base/compiler_specific.h"
#include "base/memory/weak_ptr.h"
#include "components/download/public/common/download_item.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/download_manager_delegate.h"

class AlloyBrowserHostImpl;

#if BUILDFLAG(ENABLE_CEF_DOWNLOADS)
class CefDownloadManagerDelegate : public download::DownloadItem::Observer,
                                   public content::DownloadManager::Observer,
                                   public content::DownloadManagerDelegate,
                                   public CefBrowserHostBase::Observer {
 public:
  explicit CefDownloadManagerDelegate(content::DownloadManager* manager);
  ~CefDownloadManagerDelegate() override;

 private:
  // DownloadItem::Observer methods.
  void OnDownloadUpdated(download::DownloadItem* item) override;
  void OnDownloadDestroyed(download::DownloadItem* item) override;

  // DownloadManager::Observer methods.
  void OnDownloadCreated(content::DownloadManager* manager,
                         download::DownloadItem* item) override;
  void ManagerGoingDown(content::DownloadManager* manager) override;

  // DownloadManagerDelegate methods.
  bool DetermineDownloadTarget(
      download::DownloadItem* item,
      content::DownloadTargetCallback* callback) override;
  void GetNextId(content::DownloadIdCallback callback) override;
  std::string ApplicationClientIdForFileScanning() override;

  // CefBrowserHostBase::Observer methods.
  void OnBrowserDestroyed(CefBrowserHostBase* browser) override;

  AlloyBrowserHostImpl* GetOrAssociateBrowser(download::DownloadItem* item);
  AlloyBrowserHostImpl* GetBrowser(download::DownloadItem* item);

  content::DownloadManager* manager_;
  base::WeakPtrFactory<content::DownloadManager> manager_ptr_factory_;

  // Map of DownloadItem to originating AlloyBrowserHostImpl. Maintaining this
  // map is necessary because DownloadItem::GetWebContents() may return NULL if
  // the browser navigates while the download is in progress.
  typedef std::map<download::DownloadItem*, AlloyBrowserHostImpl*>
      ItemBrowserMap;
  ItemBrowserMap item_browser_map_;

  DISALLOW_COPY_AND_ASSIGN(CefDownloadManagerDelegate);
};
#else
// Minimal delegate used when CEF download callbacks and wrappers are omitted.
// It cancels every target determination without changing the public CEF ABI.
class CefDownloadManagerDelegate : public content::DownloadManagerDelegate {
 public:
  explicit CefDownloadManagerDelegate(content::DownloadManager* manager);
  ~CefDownloadManagerDelegate() override;

 private:
  bool DetermineDownloadTarget(
      download::DownloadItem* item,
      content::DownloadTargetCallback* callback) override;
  void GetNextId(content::DownloadIdCallback callback) override;

  DISALLOW_COPY_AND_ASSIGN(CefDownloadManagerDelegate);
};
#endif

#endif  // CEF_LIBCEF_BROWSER_DOWNLOAD_MANAGER_DELEGATE_H_
