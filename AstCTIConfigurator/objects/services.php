<?php
/**
 * This file is part of AstCTIConfigurator.
 * 
 * AstCTIConfigurator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * AstCTIConfigurator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Fabrizio Balliano <fabrizio.balliano@crealabs.it>
 * @license http://www.gnu.org/licenses/gpl.html GNU General Public License
 */

/**
 * @author Fabrizio Balliano <fabrizio.balliano@crealabs.it>
 */
class services extends p4a_simple_edit_mask
{
	/**
	 * @var P4A_Array_Source
	 */
	public $service_context_types = null;
	
	/**
	 * @var P4A_Array_Source
	 */
	public $service_trigger_types = null;
	
	public function __construct()
	{
		parent::__construct("services");
		$this->fieldset->setLabel("Details about current service");
		
		$service_context_types = array();
		$service_context_types[] = array("id"=>"INBOUND", "desc"=>"INBOUND");
		$service_context_types[] = array("id"=>"OUTBOUND", "desc"=>"OUTBOUND");
		$this->build("p4a_array_source", "service_context_types")
			->load($service_context_types)
			->setPk("id");
		$this->fields->SERVICE_CONTEXT_TYPE
			->setType("select")
			->setSource($this->service_context_types);
			
		$service_trigger_types = array();
		$service_trigger_types[] = array("id"=>"NONE", "desc"=>"NONE");
		$service_trigger_types[] = array("id"=>"APPLICATION", "desc"=>"APPLICATION");
		$service_trigger_types[] = array("id"=>"BROWSER", "desc"=>"BROWSER");
		$this->build("p4a_array_source", "service_trigger_types")
			->load($service_trigger_types)
			->setPk("id");
		$this->fields->SERVICE_TRIGGER_TYPE
			->setType("select")
			->setSource($this->service_trigger_types);
			
		while ($field = $this->fields->nextItem()) {
			$field->label->setWidth(150);
		}
		
		$this->display("menu", p4a::singleton()->menu);
	}
}