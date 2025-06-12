// Copyright (c) Yohei Yoshihara. All rights reserved.

import Cocoa

class WindowController: NSWindowController, NSToolbarDelegate {
  
  override func windowDidLoad() {
    super.windowDidLoad()
    guard let window = self.window else { fatalError("no window") }
    let toolbar = NSToolbar(identifier: NSToolbar.Identifier("toolbar"))
    toolbar.delegate = self
    toolbar.displayMode = .iconOnly
    window.toolbar = toolbar
    window.toolbarStyle = .unified
    window.styleMask.insert(.fullSizeContentView)
  }

  // MARK: Toolbar
  
  struct ItemIdentifier {
    static let pushButton = NSToolbarItem.Identifier("PushButton")
    static let groupSelectOne = NSToolbarItem.Identifier("SegmentedControl")
    static let groupMomentary = NSToolbarItem.Identifier("SeparatedSegmentedControl")
    static let zoomMenu = NSToolbarItem.Identifier("PopUpButton")
    static let drawToolMenu = NSToolbarItem.Identifier("PullDownButton")
    static let searchField = NSToolbarItem.Identifier("SearchField")
  }

  let toolbarItemIdentifiers: [NSToolbarItem.Identifier] = [
//    ItemIdentifier.pushButton,
//    NSToolbarItem.Identifier.space,
    ItemIdentifier.groupSelectOne,
//    ItemIdentifier.groupMomentary,
//    ItemIdentifier.zoomMenu,
//    ItemIdentifier.drawToolMenu,
//    NSToolbarItem.Identifier.flexibleSpace,
//    ItemIdentifier.searchField,
  ]
  
  let drawTool = NSImage(systemSymbolName: "cursorarrow", accessibilityDescription: "cursor tool")
  let rectangleTool = NSImage(systemSymbolName: "rectangle", accessibilityDescription: "rectangle tool")
  let circleTool = NSImage(systemSymbolName: "circle", accessibilityDescription: "circle tool")

  func toolbar(_ toolbar: NSToolbar, itemForItemIdentifier itemIdentifier: NSToolbarItem.Identifier, willBeInsertedIntoToolbar flag: Bool) -> NSToolbarItem? {
    guard toolbarItemIdentifiers.contains(itemIdentifier) else { return nil }
    
    switch itemIdentifier {
//    case ItemIdentifier.pushButton:
//      // 単独のボタン
//      let toolbarItem = NSToolbarItem(itemIdentifier: itemIdentifier)
//      toolbarItem.image = NSImage(systemSymbolName: "pencil", accessibilityDescription: "draw tool")
//      toolbarItem.isBordered = true
//      toolbarItem.target = self
//      toolbarItem.action = #selector(selectDrawTool(_:))
//      return toolbarItem

    case ItemIdentifier.groupSelectOne:
      // ラジオボタンのようにどれか一つが選択されるボタンのグループ
      let toolbarItem = NSToolbarItemGroup(itemIdentifier: itemIdentifier,
                                           images: [drawTool!,
                                                    rectangleTool!,
                                                    circleTool!],
                                           selectionMode: .selectOne,
                                           labels: nil,
                                           target: self,
                                           action: #selector(selectSegmentedControl(_:)))
      return toolbarItem
//      
//    case ItemIdentifier.groupMomentary:
//      // グループ化された複数のボタン
//      let toolbarItem = NSToolbarItemGroup(itemIdentifier: itemIdentifier,
//                                           images: [NSImage(named: "DrawTool")!,
//                                                    NSImage(named: "EffectsTool")!,
//                                                    NSImage(named: "MoveTool")!],
//                                           selectionMode: .momentary,
//                                           labels: nil,
//                                           target: self,
//                                           action: #selector(selectSegmentedControl(_:)))
//      return toolbarItem
//
//    case ItemIdentifier.zoomMenu:
//      // テキストを表示するメニュー
//      let toolbarItem = NSMenuToolbarItem(itemIdentifier: itemIdentifier)
//      toolbarItem.title = "50%"
//      let menu = NSMenu(title: "")
//
//      let menuItem1 = NSMenuItem(title: "50%", action: #selector(selectZoomFromMenu(_:)), keyEquivalent: "")
//      menuItem1.target = self
//      menu.addItem(menuItem1)
//      
//      let menuItem2 = NSMenuItem(title: "100%", action: #selector(selectZoomFromMenu(_:)), keyEquivalent: "")
//      menuItem2.target = self
//      menu.addItem(menuItem2)
//      
//      let menuItem3 = NSMenuItem(title: "150%", action: #selector(selectZoomFromMenu(_:)), keyEquivalent: "")
//      menuItem3.target = self
//      menu.addItem(menuItem3)
//      
//      toolbarItem.menu = menu
//      return toolbarItem
//
//    case ItemIdentifier.drawToolMenu:
//      // イメージを表示するメニュー
//      let toolbarItem = NSMenuToolbarItem(itemIdentifier: itemIdentifier)
//      toolbarItem.image = NSImage(named: "DrawTool")!
//      
//      let menu = NSMenu(title: "")
//      
//      let menuItem1 = NSMenuItem(title: "", action: #selector(selectDrawToolFromMenu(_:)), keyEquivalent: "")
//      menuItem1.target = self
//      menuItem1.image = NSImage(named: "DrawTool")!
//      menu.addItem(menuItem1)
//      
//      let menuItem2 = NSMenuItem(title: "", action: #selector(selectDrawToolFromMenu(_:)), keyEquivalent: "")
//      menuItem2.target = self
//      menuItem2.image = NSImage(named: "EffectsTool")!
//      menu.addItem(menuItem2)
//      
//      let menuItem3 = NSMenuItem(title: "", action: #selector(selectDrawToolFromMenu(_:)), keyEquivalent: "")
//      menuItem3.target = self
//      menuItem3.image = NSImage(named: "MoveTool")!
//      menu.addItem(menuItem3)
//      
//      toolbarItem.menu = menu
//
//      return toolbarItem
//
//    case ItemIdentifier.searchField:
//      // 検索フィールド
//      let toolbarItem = NSSearchToolbarItem(itemIdentifier: itemIdentifier)
//      return toolbarItem

    default:
      break
    }
    
    return NSToolbarItem(itemIdentifier: itemIdentifier)
  }

  func toolbarAllowedItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {
    return toolbarItemIdentifiers
  }
  
  func toolbarDefaultItemIdentifiers(_ toolbar: NSToolbar) -> [NSToolbarItem.Identifier] {
    return toolbarItemIdentifiers
  }
  
  @objc
  func selectDrawTool(_ sender: Any) {
    //NSApplication.shared.sendAction(#selector(ViewController.doSelectTool(_:)), to: nil, from: self)
  }
  
  @objc
  func selectSegmentedControl(_ sender: NSToolbarItemGroup) {
    print("selectedIndex = \(sender.selectedIndex)")
  }

  @objc
  func selectZoomFromMenu(_ sender: Any) {
    // 選択されたアイテムに合わせてテキストを変更する
    if let menuItem = sender as? NSMenuItem,
      let item = window?.toolbar?.items.first(where: { item -> Bool in
        return item.itemIdentifier == ItemIdentifier.zoomMenu
      }) {
      item.title = menuItem.title
    }
  }
  
  @objc
  func selectDrawToolFromMenu(_ sender: Any) {
    // 選択されたアイテムに合わせて画像を変更する
    if let menuItem = sender as? NSMenuItem,
      let item = window?.toolbar?.items.first(where: { item -> Bool in
        return item.itemIdentifier == ItemIdentifier.drawToolMenu
      }) {
      item.image = menuItem.image
    }
  }

}
