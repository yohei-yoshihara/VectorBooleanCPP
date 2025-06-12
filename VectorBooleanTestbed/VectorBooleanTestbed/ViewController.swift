// Copyright (c) Yohei Yoshihara. All rights reserved. 

import Cocoa

class ViewController: NSViewController {
  lazy var scrollView = NSScrollView(frame: .zero)

  override func viewDidLoad() {
    super.viewDidLoad()

    
    
    var path = fb.FBBezierPath()
    path.moveTo(fb.FBPoint(x: 0, y: 0))
  }

  override var representedObject: Any? {
    didSet {
    // Update the view, if already loaded.
    }
  }


}

