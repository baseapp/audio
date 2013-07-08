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

package org.alljoyn.services.audio.android;

import java.util.ArrayList;
import java.util.HashMap;

import org.alljoyn.services.audio.android.SinkSelectDialog.SinkSelectDialogListener;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;

public class SinkListAdapter extends ArrayAdapter<Object> {

	private Activity mActivity;
	private static ArrayList<Object> mList = new ArrayList<Object>(); 
	private static HashMap<String, Boolean> mCheckedList = new HashMap<String, Boolean>();

	class SessionInfoHolder{
		public String name;
		public String path;
		public String friendlyName;
		public short port;

		public SessionInfoHolder(String name, String path, String friendlyName, short port){
			this.name = name;
			this.path = path;
			this.friendlyName = friendlyName;
			this.port = port;
		}
	}
	
	public SinkListAdapter(Activity activity, int textViewResourceId) {
		super(activity, textViewResourceId, mList);
		mActivity = activity;
	}
	
	private int foundLoc(String name) {
		int i = -1;
		for(Object info:mList) {
			i++;
			if(((SessionInfoHolder)info).name.equals(name))
				return i;
		}
		return -1;
	}
	
	public void add(String name, String path, String friendlyName, short port) {
		if(-1 == foundLoc(name)) {
			mList.add(new SessionInfoHolder(name, path, friendlyName, port));
			if(!mCheckedList.containsKey(name))
				mCheckedList.put(name, Boolean.FALSE);
			notifyDataSetChanged();	
		}
	}
	
	public void remove(String name) {
		int foundLoc = foundLoc(name);
		if(-1 != foundLoc) {
			mList.remove(foundLoc);
			mCheckedList.remove(name);
			notifyDataSetChanged();	
		}
	}
	
	public void clear() {
		mList.clear();
		notifyDataSetChanged();
	}
	
	@Override
	public int getCount() {
		return mList.size();
	}

	@Override
	public Object getItem(int pos) {
		return mList.get(pos);
	}
	
	public String getName(int pos) {
		return ((SessionInfoHolder)mList.get(pos)).name;
	}
	
	public String getPath(int pos) {
		return ((SessionInfoHolder)mList.get(pos)).path;
	}
	
	public short getPort(int pos) {
		return ((SessionInfoHolder)mList.get(pos)).port;
	}
	
	public void toggleSelected(int pos) {
		SessionInfoHolder info = (SessionInfoHolder)mList.get(pos);
		mCheckedList.put(info.name, Boolean.TRUE);
		if(mActivity != null) {
			mActivity.runOnUiThread(new Runnable() {
				public void run() {
					notifyDataSetChanged();	
				}
			});
		}
	}

	@Override
	public long getItemId(int id) {
		return id;
	}
	
	@Override
	public boolean areAllItemsEnabled()
    {
            return true;
    }

	@Override
    public boolean isEnabled(int post)
    {
            return true;
    } 

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		// Inflate a view template
		SessionInfoHolder info = (SessionInfoHolder)mList.get(position);
		//user == null implies my chat message
		if(mActivity != null) {
			LayoutInflater inflater = (LayoutInflater)mActivity.getSystemService (Context.LAYOUT_INFLATER_SERVICE);
			convertView = inflater.inflate(R.layout.genericitem, parent, false);
	
			final CheckBox cb = (CheckBox) convertView.findViewById(R.id.itemCheckbox);
			cb.setChecked(mCheckedList.get(info.name) == Boolean.TRUE);
			final int loc = position;
			cb.setOnClickListener(new OnClickListener() {
				public void onClick(View arg0) {
					SessionInfoHolder info = (SessionInfoHolder)mList.get(loc);
					mCheckedList.put(info.name, !mCheckedList.get(info.name));
					try {
						if(mCheckedList.get(info.name)) {
							((SinkSelectDialogListener)mActivity).onDialogSinkEnable(getName(loc), getPath(loc), getPort(loc));
						} else {
							((SinkSelectDialogListener)mActivity).onDialogSinkDisable(getName(loc));
						}
					}catch(Exception e) {
						((CheckBox)arg0).setChecked(false);
						mCheckedList.put(info.name, Boolean.FALSE);
						AlertDialog.Builder alertBuilder = new AlertDialog.Builder(mActivity);
			    		alertBuilder.setMessage("You must select a song prior to enabling speakers.\nException: "+e.getMessage()+".");
			    		alertBuilder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface arg0, int arg1) {
								arg0.dismiss();
							}
			    		});
			    		alertBuilder.create().show();
					}
				}
			});
			cb.setText(info.friendlyName); 
		}		
		return convertView;
	}
}