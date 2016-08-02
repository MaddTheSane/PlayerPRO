//
//  MasterViewController.swift
//  iOS Player
//
//  Created by C.W. Betts on 8/31/14.
//  Copyright (c) 2014 C.W. Betts. All rights reserved.
//

import UIKit

class MasterViewController: UITableViewController {
	var detailViewController: DetailViewController? = nil
	var objects = NSMutableArray()


	override func awakeFromNib() {
		super.awakeFromNib()
		if UIDevice.current.userInterfaceIdiom == .pad {
		    self.clearsSelectionOnViewWillAppear = false
		    self.preferredContentSize = CGSize(width: 320.0, height: 600.0)
		}
	}

	override func viewDidLoad() {
		super.viewDidLoad()
		// Do any additional setup after loading the view, typically from a nib.
		self.navigationItem.leftBarButtonItem = self.editButtonItem

		let addButton = UIBarButtonItem(barButtonSystemItem: .add, target: self, action: #selector(MasterViewController.insertNewObject(_:)))
		self.navigationItem.rightBarButtonItem = addButton
		if let split = self.splitViewController {
		    let controllers = split.viewControllers
		    self.detailViewController = controllers[(controllers.count - 1)] as? DetailViewController
		}
	}

	override func didReceiveMemoryWarning() {
		super.didReceiveMemoryWarning()
		// Dispose of any resources that can be recreated.
	}

	func insertNewObject(_ sender: AnyObject) {
		objects.insert(NSDate(), at: 0)
		let indexPath = IndexPath(row: 0, section: 0)
		self.tableView.insertRows(at: [indexPath], with: .automatic)
	}

	// MARK: - Segues

	override func prepare(for segue: UIStoryboardSegue, sender: AnyObject?) {
		if segue.identifier == "showDetail" {
		    let indexPath = self.tableView.indexPathForSelectedRow!
		    let object = objects[indexPath.row] as! NSDate
		    let controller = (segue.destination as! UINavigationController).topViewController as! DetailViewController
		    controller.detailItem = object
		    controller.navigationItem.leftBarButtonItem = self.splitViewController!.displayModeButtonItem
		    controller.navigationItem.leftItemsSupplementBackButton = true
		}
	}

	// MARK: - Table View

	override func numberOfSections(in tableView: UITableView) -> Int {
		return 1
	}

	override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
		return objects.count
	}

	override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
		let cell = tableView.dequeueReusableCell(withIdentifier: "Cell", for: indexPath)

		let object = objects[indexPath.row] as! NSDate
		cell.textLabel?.text = object.description
		return cell
	}

	override func tableView(_ tableView: UITableView, canEditRowAt indexPath: IndexPath) -> Bool {
		// Return false if you do not want the specified item to be editable.
		return true
	}

	override func tableView(_ tableView: UITableView, commit editingStyle: UITableViewCellEditingStyle, forRowAt indexPath: IndexPath) {
		if editingStyle == .delete {
		    objects.removeObject(at: indexPath.row)
		    tableView.deleteRows(at: [indexPath], with: .fade)
		} else if editingStyle == .insert {
		    // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view.
		}
	}


}

