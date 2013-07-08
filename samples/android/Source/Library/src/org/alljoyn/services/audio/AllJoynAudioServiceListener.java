/******************************************************************************
 * Copyright 2013, Qualcomm Innovation Center, Inc.
 *
 *    All rights reserved.
 *    This file is licensed under the 3-clause BSD license in the NOTICE.txt
 *    file for this project. A copy of the 3-clause BSD license is found at:
 *
 *        http://opensource.org/licenses/BSD-3-Clause.
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the license is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the license for the specific language governing permissions and
 *    limitations under the license.
 ******************************************************************************/

package org.alljoyn.services.audio;

public interface AllJoynAudioServiceListener {
	/**
	 * Callback that executes when a Sink has been found.
	 * 
	 * @param speakerName	Unique name of the Sink.  Pass this into other API calls.
	 * @param speakerPath	Path that the AllJoyn Audio interface is implemented.
	 * @param friendlyName	Friendly name to be used by the UI to identify a Sink.
	 * @param speakerPort	Port that is used by AllJoyn JoinSession API.
	 */
	public void SinkFound( String speakerName, String speakerPath, String friendlyName, short speakerPort );

	/**
	 * Callback that executes when a Sink is no longer available.
	 * 
	 * @param speakerName	Unique name of the Sink.
	 */
	public void SinkLost( String speakerName );

	/**
	 * Callback that executes when a Sink is ready to receive audio.
	 * 
	 * @param speakerName	Unique name of the Sink.
	 */
	public void SinkReady( String speakerName );

	/**
	 * Callback that executes when a Sink is no longer available.
	 * 
	 * @param speakerName	Unique name of the Sink.
	 * @param lost			If false implies a RemoveSink call was made, if true then Sink was lost while connected.
	 */
	public void SinkRemoved( String speakerName, boolean lost );

	/**
	 * Callback that executes when a AddSink call has failed.
	 * 
	 * @param speakerName	Unique name of the Sink.
	 */
	public void SinkError( String speakerName );
}
